package com.sensing.core.coreviewer;

import android.util.SparseArray;

public class Utilities {
    /**
     * Converts a byte array into a string of the hexadecimal repressentation.
     * @param bytes Byte-array
     * @return Converted string
     */
    public static String bytesToHexString(byte[] bytes) {
        char[] hexArray = "0123456789ABCDEF".toCharArray();
        char[] hexChars = new char[bytes.length * 2];
        for ( int j = 0; j < bytes.length; j++ ) {
            int v = bytes[j] & 0xFF;
            hexChars[j * 2] = hexArray[v >>> 4];
            hexChars[j * 2 + 1] = hexArray[v & 0x0F];
        }
        return "0x" + new String(hexChars);
    }

    /**
     * Convert SparseArray to String for debugging purposes.
     * @param array SparseArray with BLE-advertisement-record
     * @return SparseArray as String
     */
    public static String sparseToString(SparseArray<byte[]> array) {
        if (array == null) {
            return "null";
        }
        if (array.size() == 0) {
            return "";
        }
        StringBuilder buffer = new StringBuilder();
        for (int i = 0; i < array.size(); ++i) {
            buffer.append(bytesToHexString(array.valueAt(i)));
        }
        return buffer.toString();
    }
}
