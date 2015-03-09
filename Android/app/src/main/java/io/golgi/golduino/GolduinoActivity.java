package io.golgi.golduino;

import android.os.Handler;
import android.os.Message;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.RelativeLayout;
import android.widget.Switch;
import android.widget.TextView;

import java.util.Timer;
import java.util.TimerTask;
import com.openmindnetworks.golgi.api.GolgiAPI;
import com.openmindnetworks.golgi.api.GolgiException;
import com.openmindnetworks.golgi.api.GolgiTransportOptions;

import io.golgi.api.GolgiPersistentHash;
import io.golgi.golduino.gen.GolDuinoService;
import io.golgi.golduino.gen.IOState;
import io.golgi.golduino.gen.PinSetting;


public class GolduinoActivity extends ActionBarActivity {
    private boolean inFg = false;
    private GolgiPersistentHash gpHash;
    SignalMeter signalMeter;
    View spinnerView;
    Switch ledSwitch;
    ImageView ledOnIv, ledOffIv;
    Button buzzerButton;
    boolean buzzerPressed = false;
    boolean ledChecked = false;
    public static int potValue = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_golduino);
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_golduino, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    int inFlight = 0;

    Handler uiFixupHandler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            if(ledSwitch.isChecked()){
                ledOffIv.setVisibility(View.GONE);
                ledOnIv.setVisibility(View.VISIBLE);
            }
            else{
                ledOnIv.setVisibility(View.GONE);
                ledOffIv.setVisibility(View.VISIBLE);
            }
            if(inFlight > 0){
                spinnerView.setVisibility(View.VISIBLE);
                signalMeter.setSignalLevel(50);
                signalMeter.invalidate();

            }
            else {
                spinnerView.setVisibility(View.INVISIBLE);
                signalMeter.setSignalLevel(0);
                signalMeter.invalidate();
            }
        }
    };

    void sortOutLedImage(){
        uiFixupHandler.sendEmptyMessage(0);
    }

    private void sendState(){
        DBG.write("Send state LED: " + (ledChecked ? "ON " : "OFF") + " Buzzer: " + (buzzerPressed ? "ON" : "OFF"));
        IOState ioState = new IOState();

        ioState.setLedState(ledChecked ? 1 : 0);

        // ioState.setBuzzerState(buzzerPressed ? 1 : 0);

        GolgiTransportOptions gto = new GolgiTransportOptions();
        gto.setValidityPeriod(120);
        inFlight++;
        sortOutLedImage();
        GolDuinoService.setIO.sendTo(
                new GolDuinoService.setIO.ResultReceiver() {
                    @Override
                    public void failure(GolgiException ex) {
                        DBG.write("FAILURE: " + ex.getErrText());
                        inFlight--;
                        sortOutLedImage();
                    }

                    @Override
                    public void success(){
                        inFlight--;
                        sortOutLedImage();
                        DBG.write("SUCCESS");
                    }
                },
                gto,
                // GolgiService.getGolgiId(GolduinoActivity.this),
                "BK-DUINO",
                ioState);

    }

    private Timer refreshTimer;
    private Handler redrawHandler = new Handler(){
        public void handleMessage(Message msg){
            if(inFg){
                if(signalMeter != null) {
                    signalMeter.setSignalLevel((potValue  * 100) / 1024);
                    signalMeter.invalidate();
                }
            }
        }
    };

    private void startRefreshTimer(){
        signalMeter = (SignalMeter)findViewById(R.id.signalMeter);
        if(refreshTimer == null){
            DBG.write("Starting refreshTimer()");
            refreshTimer = new Timer();
            refreshTimer.schedule(new TimerTask(){
                @Override
                public void run() {
                    redrawHandler.sendEmptyMessage(0);
                }
            }, 20, 20);
        }
    }

    private void stopRefreshTimer(){
        if(refreshTimer != null){
            DBG.write("Stopping refreshTimer()");
            refreshTimer.cancel();
            refreshTimer = null;
        }
    }


    @Override
    public void onResume() {
        DBG.write("onResume()");
        super.onResume();
        inFg = true;
        GolgiAPI.usePersistentConnection();

        if (!GolgiService.isRunning(this)) {
            DBG.write("Start the service");
            GolgiService.startService(this.getApplicationContext());
        }
        else {
            gpHash = GolgiService.getGpHash();
            DBG.write("Service already started");
        }

        signalMeter = (SignalMeter)findViewById(R.id.signalMeter);
        signalMeter.setSignalLevel(50);
        startRefreshTimer();

        spinnerView = findViewById(R.id.spinnerView);
        spinnerView.setVisibility(View.INVISIBLE);

        ledSwitch = (Switch)findViewById(R.id.switch1);

        ledSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                DBG.write("Switch is now: " + ((isChecked) ? "Checked" : "Unchecked"));
                ledChecked = isChecked;
                sortOutLedImage();
                sendState();
            }
        });

        ledOnIv = (ImageView)findViewById(R.id.ledOnIv);
        ledOffIv = (ImageView)findViewById(R.id.ledOffIv);

        sortOutLedImage();

        /*
        Button buzzerButton = (Button)findViewById(R.id.buzzerButton);

        buzzerButton.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if(event.getAction() == MotionEvent.ACTION_DOWN) {
                    if (false) {
                        DBG.write("DOWN");
                        if (!buzzerPressed) {
                            buzzerPressed = true;
                            sendState();
                        }
                    } else if (event.getAction() == MotionEvent.ACTION_UP) {
                        DBG.write("UP");
                        if (buzzerPressed) {
                            buzzerPressed = false;
                            sendState();
                        }
                    }
                }
                return false;
            }
        });
        */

        /*
        Button b = (Button)findViewById(R.id.sendButton);
        b.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                DBG.write("SEND PRESSED");
                PinSetting ps = new PinSetting();
                ps.setPin(1);
                ps.setVal(10);
                ps.setStr("Mary\nHad\nA\nLittle\nLamb");
                GolgiTransportOptions gto = new GolgiTransportOptions();
                gto.setValidityPeriod(60);
                GolDuinoService.setPin.sendTo(
                        new GolDuinoService.setPin.ResultReceiver() {
                            @Override
                            public void failure(GolgiException ex) {
                                DBG.write("FAILURE: " + ex.getErrText());
                            }

                            @Override
                            public void success(PinSetting result) {
                                DBG.write("SUCCESS: " + result.getPin() + "/" + result.getVal());
                            }
                        },
                        gto,
                        // GolgiService.getGolgiId(GolduinoActivity.this),
                        "BK-DUINO",
                        ps);
            }
        });
        */
    }

    @Override
    public void onPause() {
        DBG.write("onPause()");
        super.onPause();
        inFg = false;
        stopRefreshTimer();
        GolgiAPI.useEphemeralConnection();
    }

}
