package io.golgi.golduino;

import android.util.Log;

/**
 * Created by briankelly on 05/02/15.
 */
public class DBG {
    public static void write(String where, String str){
        Log.i(where, str);
    }

    public static void write(String str){
        write("GOLDUINO", str);
    }
}
