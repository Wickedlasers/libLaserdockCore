package com.wickedlasers.laserdocklib;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbEndpoint;
import android.hardware.usb.UsbInterface;
import android.hardware.usb.UsbManager;
import android.hardware.usb.UsbRequest;
import android.util.Log;

import java.nio.Buffer;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

/**
 * USB android Helper for laserdock library
 */
public class LdUsbDeviceHelper {

    private static String TAG = "laserdocklib_UsbDeviceHelper";
    private static final String ACTION_USB_PERMISSION =
            "com.wickedlasers.laserdock.USB_PERMISSION";
    private static boolean m_isRequestingPermission = false;
    public static LaserdockUsbCmdConnection mCmdConnection;
    public static LaserdockUsbDataConnection mDataConnection;

    private static final BroadcastReceiver mUsbReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (ACTION_USB_PERMISSION.equals(action)) {
                synchronized (this) {
                    m_isRequestingPermission = false;

                    UsbManager manager = (UsbManager) context.getSystemService(Context.USB_SERVICE);
                    UsbDevice device = intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
                    Log.d(TAG, "interface count : "+device.getInterfaceCount());
                    if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
                    } else {
                        Log.d(TAG, "permission denied for device " + device);
                    }
                }
            }
        }

    };

    public static final void registerUSBPermissionReceiver(Context context) {
        UsbManager manager = (UsbManager) context.getSystemService(Context.USB_SERVICE);

        IntentFilter filter = new IntentFilter(ACTION_USB_PERMISSION);
        context.registerReceiver(mUsbReceiver, filter);
    }

    public static UsbDevice[] getLaserdockDevices(Context context) {
        List<UsbDevice> devices = new ArrayList<UsbDevice>();

        UsbManager manager = (UsbManager) context.getSystemService(Context.USB_SERVICE);
        HashMap<String, UsbDevice> deviceMap = manager.getDeviceList();


        // filter for laserdock only
        for (UsbDevice device : deviceMap.values()) {
            if(isLaserdockDevice(device)) {
                devices.add(device);
                if(!manager.hasPermission(device)) {
                    PendingIntent permission_intent = PendingIntent.getBroadcast(context, 0, new Intent(ACTION_USB_PERMISSION), 0);
                    manager.requestPermission(device, permission_intent);
                }
            }
        }

        return devices.toArray(new UsbDevice[devices.size()]);
    }

    public static UsbDeviceConnection openDevice(Context context, UsbDevice device) {
        UsbManager manager = (UsbManager) context.getSystemService(Context.USB_SERVICE);
        // check permission
        if (manager.hasPermission(device)) {
            // ok, open
            return doOpenDevice(context, device);
        } else {
            if(m_isRequestingPermission) {
                return null;
            }

            // request permission
            PendingIntent pi = PendingIntent.getBroadcast(context, 0, new Intent(
                    ACTION_USB_PERMISSION), 0);
            manager.requestPermission(device, pi);
            m_isRequestingPermission = true;
            return null;
        }
    }


    private static UsbDeviceConnection doOpenDevice(Context context, UsbDevice device) {
        UsbManager manager = (UsbManager) context.getSystemService(Context.USB_SERVICE);
        try {
            UsbDeviceConnection connection = manager.openDevice(device);
            return connection;
//            Log.d(TAG, "device was opened, fd:" + connection.getFileDescriptor());
//            return connection.getFileDescriptor();
        } catch (SecurityException exc) {
            // should never happen because we should have access at this point
            exc.printStackTrace();
        }

        return null;
    }

    private static boolean isLaserdockDevice(UsbDevice device) {
        int LASERDOCK_VIN = 0x1fc9;
        int LASERDOCK_PIN = 0x04d8;
        return device.getProductId() == LASERDOCK_PIN && device.getVendorId() == LASERDOCK_VIN;
    }


    public static int setupDevice(Context context, UsbDevice device){
        UsbManager manager = (UsbManager) context.getSystemService(Context.USB_SERVICE);
        UsbDeviceConnection connection0 = manager.openDevice(device);
        UsbDeviceConnection connection1 = manager.openDevice(device);

        if(connection0 == null || connection1 == null) {
            return -1;
        }
//        Log.d(TAG, "device was setup, fd " + connection0.getFileDescriptor() + " " + connection1.getFileDescriptor());
        mCmdConnection = new LaserdockUsbCmdConnection(device, connection0);
        mDataConnection = new LaserdockUsbDataConnection(device, connection1);

        return 0;
    }


}
