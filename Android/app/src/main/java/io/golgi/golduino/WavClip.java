package io.golgi.golduino;

/**
 * Created by brian on 2/25/15.
 */
public class WavClip {
    private String srcUid;
    private String dstUid;
    private short[] audioData;
    private long timestamp;

    public short[] getAudioData(){
        return audioData;
    }

    public String getSrcUid(){
        return srcUid;
    }

    public String getDstUid(){
        return dstUid;
    }

    public long getTimestamp(){
        return timestamp;
    }

    WavClip(String srcUid, String dstUid, short[] audioData){
        this.srcUid = srcUid;
        this.dstUid = dstUid;
        this.audioData = audioData;
        this.timestamp = System.currentTimeMillis();
    }

}
