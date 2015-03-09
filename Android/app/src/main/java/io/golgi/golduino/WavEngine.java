package io.golgi.golduino;

import android.content.Context;
import android.content.SharedPreferences;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.media.Ringtone;
import android.media.RingtoneManager;
import android.net.Uri;
import android.preference.PreferenceManager;
import android.provider.Settings;

import java.util.Timer;
import java.util.TimerTask;
import java.util.Vector;

/**
 * Created by brian on 2/25/15.
 */



public class WavEngine {
    private Object syncObj = new Object();
    private Ringtone vNoteRingtone;
    private AudioTrack vNoteAudioTrack;
    private WavClip mostRecentWavClip;
    private WavClip curVNote;
    private Vector<WavClip> vNoteQueue = new Vector<WavClip>();
    private Timer vNoteTimer;
    private int originalVolume;
    private GolgiService ctrlr;

    private void DBG(String str){
        DBG.write("SVC", str);
    }


    private void startVNoteTimer(){
        synchronized(syncObj){
            if(vNoteTimer == null){
                vNoteTimer = new Timer();
                AudioManager audioManager = (AudioManager)ctrlr.getSystemService(Context.AUDIO_SERVICE);
                originalVolume = audioManager.getStreamVolume(AudioManager.STREAM_MUSIC);

                vNoteTimer.schedule(new TimerTask(){
                    @Override
                    public void run() {
                        synchronized(syncObj){
                            if(vNoteRingtone != null){
                                if(!vNoteRingtone.isPlaying()){
                                    DBG("Ringtone complete");
                                    vNoteRingtone = null;
                                    vNoteAudioTrack.play();
                                }
                            }
                            else if(vNoteAudioTrack != null){
                                int pos = vNoteAudioTrack.getPlaybackHeadPosition();
                                // DBG("Playback Position: " + pos + "(" + curVNote.getAudioData().length + ")");
                                if(pos == curVNote.getAudioData().length){
                                    vNoteAudioTrack.stop();
                                    vNoteAudioTrack = null;
                                    mostRecentWavClip = curVNote;
                                    curVNote = null;
                                }
                            }

                            if(vNoteRingtone == null && vNoteAudioTrack == null){
                                if(vNoteQueue.size() == 0){
                                    vNoteTimer.cancel();
                                    vNoteTimer = null;
                                    AudioManager audioManager = (AudioManager)ctrlr.getSystemService(Context.AUDIO_SERVICE);
                                    audioManager.setStreamVolume(AudioManager.STREAM_MUSIC, originalVolume, 0);
                                }
                                else{
                                    try {
                                        curVNote = vNoteQueue.remove(0);
                                        SharedPreferences sharedPrefs = PreferenceManager.getDefaultSharedPreferences(ctrlr);
                                        String uriStr;
                                        uriStr = sharedPrefs.getString("vNoteSound", "");
                                        if(uriStr.length() == 0){
                                            uriStr = Settings.System.DEFAULT_NOTIFICATION_URI.toString();
                                        }

                                        vNoteRingtone = RingtoneManager.getRingtone(ctrlr, Uri.parse(uriStr));
                                        vNoteRingtone.play();
                                        DBG("Playing Ringtone");

                                        AudioManager audioManager = (AudioManager)ctrlr.getSystemService(Context.AUDIO_SERVICE);
                                        originalVolume = audioManager.getStreamVolume(AudioManager.STREAM_MUSIC);

                                        audioManager.setStreamVolume(AudioManager.STREAM_MUSIC, audioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC), 0);

                                        try {
                                            vNoteAudioTrack = new AudioTrack(
                                                    AudioManager.STREAM_MUSIC,
                                                    16000,
                                                    AudioFormat.CHANNEL_CONFIGURATION_MONO,
                                                    AudioFormat.ENCODING_PCM_16BIT,
                                                    (int)curVNote.getAudioData().length *2,
                                                    AudioTrack.MODE_STATIC);
                                            int rc = vNoteAudioTrack.write(curVNote.getAudioData(), 0, curVNote.getAudioData().length);
                                            DBG("Wrote " + rc);
                                            // audioTrack.setStereoVolume(1.0f, 1.0f);
                                            // vNoteAudioTrack.play();                                          // Play the track

                                        }
                                        catch (Exception e){
                                            DBG("Audio playing exploded");
                                        }
                                        DBG("Audio playing may or may not have worked");
                                    }
                                    catch (Exception e){
                                    }
                                }

                            }
                        }
                    }
                                    },
                        0, 20);
            }
        }
    }

    public void play(WavClip vNote){
        synchronized(syncObj){
            vNoteQueue.add(vNote);
            startVNoteTimer();
        }
    }

    public WavEngine(GolgiService ctrlr){
        this.ctrlr = ctrlr;
    }

}
