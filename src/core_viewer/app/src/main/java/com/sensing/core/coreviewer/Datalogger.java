package com.sensing.core.coreviewer;

import android.content.Context;
import android.os.Environment;
import android.util.Log;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;

// TODO: Use SD-card device independent

public class Datalogger {
    private static final String TAG = "DATALOGGER";
    private Context mContext;
    private String mFilename;
//    private File mDirectory;
    private String mSeperator = "; ";
    private HashMap<String, File> mAddressToFile = new HashMap<>();
    private static final int UPDATE_FILESIZE_MODULO = 500;
    private HashMap<File, Long> mFileToCurrentSize = new HashMap<>();
    private File mCurrentFolder = null;
    private int sizeUpdateCounter = 0;
    private boolean mLogIncrement = false;

    private Datalogger.Listener mParent;

    public Datalogger(Context context, Datalogger.Listener parent, boolean logIncrement) {
        mContext = context;
//        mDirectory = getPublicDocumentStorageDir();
        mParent = parent;
        mLogIncrement = logIncrement;
    }

    /* Checks if external storage is available for read and write */
    private boolean isExternalStorageWritable() {
        String state = Environment.getExternalStorageState();
        if (Environment.MEDIA_MOUNTED.equals(state)) {
            return true;
        }
        return false;
    }

    /* Checks if external storage is available to at least read */
    private boolean isExternalStorageReadable() {
        String state = Environment.getExternalStorageState();
        if (Environment.MEDIA_MOUNTED.equals(state) ||
                Environment.MEDIA_MOUNTED_READ_ONLY.equals(state)) {
            return true;
        }
        return false;
    }

    /**
     * Get path to public internal storage of app.
     * @return Path to public internal storage
     */
    private File getPublicDocumentStorageDir(String subfolder) {
//        // Get the directory for the user's public pictures directory.
//        File file = new File(Environment.getExternalStoragePublicDirectory(
//                Environment.DIRECTORY_DOCUMENTS), documentName);
//        Log.d(TAG, "filename: " + file.getAbsolutePath());
//        if (!file.mkdirs()) {
//            Log.e(TAG, "Directory not created");
//        }
//        return file;
            File basePath = mContext.getExternalFilesDir(null);
            File path = new File(basePath, subfolder);

        if(!path.exists()) {
            path.mkdirs();
        }
        return path;
    }



    /**
     * Create file if it does not already exist.
     * @param fileName Name of file
     * @return  File-handle to created file.
     */
    private File createFile(String fileName) {

        File folder = getFolder();

        File file = new File(folder, fileName);
        if(!file.exists()){
            try {
                file.createNewFile();

            } catch (IOException e) {
                Log.e(TAG, "createFile: " + e.getMessage());
                e.printStackTrace();
            }

        }
        Log.d(TAG, "createFile: created file: " + file);
        return file;
    }

    public File getFolder() {
        if(mCurrentFolder != null) {
            return mCurrentFolder;
        } else {
            Date date = new Date();
            SimpleDateFormat sdfm = new SimpleDateFormat();
            sdfm.applyPattern("HH'-'mm'-'ss'_'dd'-'MM'-'yyyy");
            String startTime = sdfm.format(date);
            mCurrentFolder = getPublicDocumentStorageDir(startTime);
            return mCurrentFolder;
        }
    }

    private void writeFile(File file, String text) {
        if(file.exists()){
            try {
                // update file size information periodically
                if((sizeUpdateCounter %UPDATE_FILESIZE_MODULO)==0){
                    long currentSize = file.length();
                    Log.d(TAG, "writeFile: size: " + currentSize);
                    mFileToCurrentSize.put(file, currentSize);
                    mParent.onFileSizeUpdate(getCurrentLogFileSize());
                }
                FileWriter fileWriter  = new FileWriter(file, true);
                BufferedWriter bfWriter = new BufferedWriter(fileWriter);
                bfWriter.write(text);
                bfWriter.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }


    public void open() {

    }

    public void close() {

    }

    public void log(String address, String increment, String value) {
        String time = String.valueOf(System.currentTimeMillis());
        String line;
        if(mLogIncrement){
            line = time + mSeperator
                    + increment + mSeperator
                    + value + "\n";
        } else {
            line = time + mSeperator
                    + value + "\n";
        }
        File file = getFile(address);
        writeFile(file, line);

        sizeUpdateCounter++;
    }

//    public void log(double time, String increment, String value) {
////        log(address, String.valueOf(System.currentTimeMillis()), increment, value);
////        log(address, String.valueOf(System.currentTimeMillis()), increment, value);
//        String line = time + mSeperator
//                + increment + mSeperator
//                + value + "\n";
//        writeFile(mFile, line);
//    }

    /**
     * Return the file to log to for a given address
     * @param deviceAddress Address of the device.
     * @return File handle to according log-file.
     */
    private File getFile(String deviceAddress) {
        if(mAddressToFile.containsKey(deviceAddress)){
            return mAddressToFile.get(deviceAddress);
        } else {
            File file = createFile(createFileName(deviceAddress));
            DataManager dm = DataManager.getInstance();
            String addressQuoted = surroundWithQuotes(deviceAddress);
            String name = surroundWithQuotes(dm.getName().get(deviceAddress));
            String calibration = surroundWithQuotes(dm.getCalibrationValue().get(deviceAddress).toString());
            String unit = surroundWithQuotes(dm.getCalibrationUnit().get(deviceAddress));
            Date date = new Date();
            SimpleDateFormat sdfm = new SimpleDateFormat();
            sdfm.applyPattern("HH'-'mm'-'ss'_'dd'-'MM'-'yyyy");
            String startTime = surroundWithQuotes(sdfm.format(date));

            // actually write data to file
            writeHeader(file, addressQuoted, name, calibration, unit, startTime);
            mAddressToFile.put(deviceAddress, file);
            return file;
        }
    }

    private String surroundWithQuotes(String s) {
        return "\"" + s + "\"";
    }

    private String createFileName(String address) {
        String sensorName = DataManager.getInstance().getName().get(address);
        String fileName = sensorName + "_" + address +".csv" ;
        return fileName;
    }

    private void writeHeader(File file, String address, String name, String calibration, String unit, String startTime ) {
        FileWriter fileWriter  = null;
        try {
            fileWriter = new FileWriter(file, true);
            BufferedWriter bfWriter = new BufferedWriter(fileWriter);
            String line;
            if(mLogIncrement) {
                line = "timestamp" + mSeperator
                        + "increment" + mSeperator
                        + "value" + mSeperator
                        + "address=" + address + mSeperator
                        + "sensorName=" + name + mSeperator
                        + "calibration=" + calibration + mSeperator
                        + "unit=" + unit + mSeperator
                        + "startTime=" + startTime + mSeperator
                        + "\n";
            } else {
                line = "timestamp" + mSeperator
                        + "value" + mSeperator
                        + "address=" + address + mSeperator
                        + "sensorName=" + name + mSeperator
                        + "calibration=" + calibration + mSeperator
                        + "unit=" + unit + mSeperator
                        + "startTime=" + startTime + mSeperator
                        + "\n";
            }
            bfWriter.write(line);
            bfWriter.close();
            bfWriter.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public Context getmContext() {
        return mContext;
    }

    public void setmContext(Context mContext) {
        this.mContext = mContext;
    }

    public String getmFilename() {
        return mFilename;
    }

    public void setmFilename(String mFilename) {
        this.mFilename = mFilename;
    }

    /**
     * Forget about everything so far. Create new files etc...
     */
    public void reset() {
        mAddressToFile.clear();
        mCurrentFolder = null;
    }

    /**
     * Get size of all log files that are currently logged to.
     * @return Size in byte.
     */
    public long getCurrentLogFileSize() {
        long size = 0;
        for(long fSize : mFileToCurrentSize.values()) {
            size += fSize;
        }
        return size;
    }

    public interface Listener {
        void onFileSizeUpdate(long fileSize);
    }
}
