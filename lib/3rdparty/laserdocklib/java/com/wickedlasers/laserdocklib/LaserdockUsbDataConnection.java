package com.wickedlasers.laserdocklib;

import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbEndpoint;
import android.hardware.usb.UsbInterface;
import android.hardware.usb.UsbRequest;
import android.util.Log;

import java.nio.ByteBuffer;

/**
 * Created by ping on 12/31/17.
 */


class LaserdockUsbDataConnection {
    private ByteBuffer mbuffer = ByteBuffer.allocateDirect(64);
    private UsbDevice mDevice;
    private UsbDeviceConnection mConnection;
    private UsbRequest mOutRequest;
    private int mdatalength = 0;

    private static String TAG = "laserdocklib_LaserdockUsbDataConnection";

    public LaserdockUsbDataConnection (UsbDevice d, UsbDeviceConnection c){
        mDevice = d;
        mConnection = c;

        Log.d(TAG, "intface count:" + mDevice.getInterfaceCount());

        UsbInterface intf = mDevice.getInterface(2);
//        Log.d(TAG, "intface :" + intf);
//        Log.d(TAG, "endpoint count :" + intf.getEndpointCount());

        UsbEndpoint ep = intf.getEndpoint(0);

//        Log.d(TAG, "endpoint  :" + ep.getDirection());

        mConnection.claimInterface(intf, true);
        mConnection.setInterface(intf);

        mOutRequest = new UsbRequest();
        mOutRequest.initialize(mConnection, ep);
    }

    public boolean send(){
//        Log.d(TAG, "sending...");
        if(!mOutRequest.queue(mbuffer, mdatalength))
            return false;

        mConnection.requestWait();

        return true;
    }

    public int fd(){
        return mConnection.getFileDescriptor();
    }
}
