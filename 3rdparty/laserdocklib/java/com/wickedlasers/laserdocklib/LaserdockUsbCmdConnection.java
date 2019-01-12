package com.wickedlasers.laserdocklib;

import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbInterface;
import android.hardware.usb.UsbRequest;
import android.util.Log;

import java.nio.ByteBuffer;

/**
 * Created by ping on 12/31/17.
 */
public class LaserdockUsbCmdConnection {
    private ByteBuffer mbuffer = ByteBuffer.allocateDirect(64);

    public UsbDevice mDevice;
    private UsbDeviceConnection mConnection;
    private UsbRequest mOutRequest;
    private UsbRequest mInRequest;

    private static String TAG = "laserdocklib_LaserdockUsbCmdConnection";

    public LaserdockUsbCmdConnection (UsbDevice d, UsbDeviceConnection c){
        mDevice = d;
        mConnection = c;
        UsbInterface intf = mDevice.getInterface(0);
        mConnection.claimInterface(intf, true);

        mOutRequest = new UsbRequest();
        mOutRequest.initialize(mConnection, intf.getEndpoint(0));
        mInRequest = new UsbRequest();
        mInRequest.initialize(mConnection, intf.getEndpoint(1));
    }

    public boolean send(){
//        Log.d(TAG, "sending...");
        if(!mOutRequest.queue(mbuffer, 64))
            return false;

        mConnection.requestWait();

        if(!mInRequest.queue(mbuffer, 64))
            return false;

        mConnection.requestWait();

        return true;
    }

    public int fd(){
        return mConnection.getFileDescriptor();
    }
}
