package io.golgi.golduino;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.media.MediaPlayer;
import android.os.PowerManager;
import android.support.v4.app.NotificationCompat;

import com.openmindnetworks.golgi.api.GolgiAPI;
import com.openmindnetworks.golgi.api.GolgiAPIHandler;

import io.golgi.apiimpl.android.GolgiAbstractService;
import io.golgi.api.GolgiPersistentHash;
import io.golgi.api.GolgiStore;
import io.golgi.golduino.gen.GolDuinoService;
import io.golgi.golduino.gen.GolgiKeys;
import io.golgi.golduino.gen.IOState;
import io.golgi.golduino.gen.PinSetting;

/**
 * Created by briankelly on 05/02/15.
 */
public class GolgiService extends GolgiAbstractService{
    private static Object syncObj = new Object();
    private static GolgiService theInstance = null;
    private GolgiPersistentHash gpHash;
    private WavEngine wavEngine;
    private MediaPlayer mediaPlayer;

    public static GolgiService getServiceInstance(){
        return theInstance;
    }

    private static void DBG(String str){
        DBG.write("GOLDUINO", str);
    }

    public static String getGolgiId(Context context) {
        String golgiId = GolgiStore.getString(context, "GOLDUINO-GOLGI-ID", "");
        if (golgiId.length() == 0) {
            StringBuffer sb = new StringBuffer();
            for (int i = 0; i < 20; i++) {
                sb.append((char) ('A' + (int) (Math.random() * ('z' - 'A'))));
            }

            golgiId = sb.toString();
            GolgiStore.putString(context, "GOLDUINO-GOLGI-ID", golgiId);
        }
        return golgiId;
    }

    public static GolgiPersistentHash getGpHash(){
        synchronized(syncObj) {
            if (theInstance != null) {
                return theInstance.gpHash;
            }
        }
        return null;
    }

    private GolDuinoService.setPin.RequestReceiver incomingSetPin = new GolDuinoService.setPin.RequestReceiver() {
        @Override
        public void receiveFrom(GolDuinoService.setPin.ResultSender resultSender, PinSetting setting) {
            setting.setPin(setting.getPin() + 1);
            setting.setVal(setting.getVal() + 1);

            resultSender.success(setting);
        }
    };

    private GolDuinoService.setIO.RequestReceiver incomingSetIO = new GolDuinoService.setIO.RequestReceiver() {
        @Override
        public void receiveFrom(GolDuinoService.setIO.ResultSender resultSender, IOState ioState) {
            DBG("Incoming setIO");
            resultSender.success();
        }
    };

    private GolDuinoService.buttonPressed.RequestReceiver incomingButtonPressed  = new GolDuinoService.buttonPressed.RequestReceiver() {
        @Override
        public void receiveFrom(GolDuinoService.buttonPressed.ResultSender resultSender, int t) {
            DBG("Incoming buttonPressed(" + t + ")");
            resultSender.success();
            // MediaPlayer mediaPlayer= MediaPlayer.create(GolgiService.this, R.raw.ding);
            // mediaPlayer.start();
        }
    };

    private GolDuinoService.buttonReleased.RequestReceiver incomingButtonReleased = new GolDuinoService.buttonReleased.RequestReceiver() {
        @Override
        public void receiveFrom(GolDuinoService.buttonReleased.ResultSender resultSender, int t) {
            DBG("Incoming buttonReleased(" + t + ")");
            resultSender.success();
            mediaPlayer = MediaPlayer.create(GolgiService.this, R.raw.dingdong);
            mediaPlayer.start();

            String ns = Context.NOTIFICATION_SERVICE;
            NotificationManager notificationManager = (NotificationManager)theInstance.getSystemService(ns);
            Intent intent = new Intent(context, GolduinoActivity.class);
            PendingIntent contentIntent = PendingIntent.getActivity(theInstance.context, 0, intent, 0);

            NotificationCompat.Builder builder;

            builder = new NotificationCompat.Builder(theInstance.context).setContentTitle("GolDuino").setContentText("Somebody is at the door.").
                    setSmallIcon(R.drawable.ic_stat_arduino_icon).
                    setContentIntent(contentIntent);


            Notification notification = builder.getNotification();
            notification.flags |= Notification.FLAG_AUTO_CANCEL;
            notification.priority = Notification.PRIORITY_MAX;

            notificationManager.notify(201, notification);

            PowerManager pm = (PowerManager)context.getSystemService(Context.POWER_SERVICE);

            boolean isScreenOn = pm.isScreenOn();

            if(isScreenOn==false)
            {

                PowerManager.WakeLock wl = pm.newWakeLock(PowerManager.FULL_WAKE_LOCK |PowerManager.ACQUIRE_CAUSES_WAKEUP |PowerManager.ON_AFTER_RELEASE,"MyLock");

                wl.acquire(10000);
                PowerManager.WakeLock wl_cpu = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK,"MyCpuLock");

                wl_cpu.acquire(10000);
            }
        }
    };

    private GolDuinoService.setPotValue.RequestReceiver incomingSetPotValue = new GolDuinoService.setPotValue.RequestReceiver() {
        @Override
        public void receiveFrom(GolDuinoService.setPotValue.ResultSender resultSender, int v) {
            DBG.write("POT Value: " + v);
            GolduinoActivity.potValue = v;
            resultSender.success();
        }
    };

    public void readyForRegister(){
        synchronized(syncObj){
            if(theInstance != null && theInstance != this){
                DBG("**************************");
                DBG("Changing Service Instance");
                DBG("**************************");
            }
            theInstance = this;
            wavEngine = new WavEngine(this);
        }

        gpHash = new GolgiPersistentHash(context, "instanceFilters");

        GolDuinoService.setPin.registerReceiver(incomingSetPin);
        GolDuinoService.setIO.registerReceiver(incomingSetIO);
        GolDuinoService.buttonPressed.registerReceiver(incomingButtonPressed);
        GolDuinoService.buttonReleased.registerReceiver(incomingButtonReleased);
        GolDuinoService.setPotValue.registerReceiver(incomingSetPotValue);

        GolgiAPI.setOption("USE_TEST_SERVER", "1");

        registerGolgi(new GolgiAPIHandler() {
                          @Override
                          public void registerSuccess() {
                              DBG.write("register SUCCESS");
                          }


                          @Override
                          public void registerFailure() {
                              DBG.write("register FAIL");
                          }
                      },
                GolgiKeys.DEV_KEY,
                GolgiKeys.APP_KEY,
                getGolgiId(this));
    }
}
