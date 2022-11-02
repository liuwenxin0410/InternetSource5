package com.mqbest.myopencv;

import androidx.appcompat.app.AppCompatActivity;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.RectF;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.text.SpannableStringBuilder;
import android.util.Log;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.hoho.android.usbserial.driver.UsbSerialDriver;
import com.hoho.android.usbserial.driver.UsbSerialPort;
import com.hoho.android.usbserial.driver.UsbSerialProber;
import com.hoho.android.usbserial.util.SerialInputOutputManager;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraActivity;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Mat;
import org.opencv.core.Rect;
import org.opencv.core.RotatedRect;
import org.opencv.core.Scalar;
import org.opencv.imgproc.Imgproc;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.Collections;
import java.util.List;

public class ObjectTrackerActivity extends CameraActivity implements CvCameraViewListener2 {
    private static final String TAG = "ObjectTracker";

    /* OpencV 部分 */
    private static final Scalar OBJECT_RECT_COLOR = new Scalar(0, 255, 0, 255);
    private Mat mRgba, mGray;
    private Rect mTrackWindow = null, mTrackerRect = null;
    private ObjectTracker mObjectTracker = null;
    private CameraBridgeViewBase mOpenCvCameraView = null;
    private int cameraindex = CameraBridgeViewBase.CAMERA_ID_BACK;
    /*********/

    /* 串口驱动部分 */
    private enum UsbPermission { Unknown, Requested, Granted, Denied }
    public static final String INTENT_ACTION_GRANT_USB = BuildConfig.APPLICATION_ID + ".GRANT_USB";
    private SerialInputOutputManager usbIoManager = null;
    private UsbSerialPort usbSerialPort = null;
    private UsbPermission usbPermission = UsbPermission.Unknown;
    private boolean connected = false;

    private Handler serialLooper = null;
    /*********/

    FrameLayout mViewContainter = null;
    private ImageView mTrackImageIv = null;
    private Handler mHandlerUI = new Handler();

    private Runnable mUpdateUI = new Runnable() {
        @Override
        public void run() {
            int x = mTrackerRect.x;
            int y = mTrackerRect.y;
            int w = mTrackerRect.width;
            int h = mTrackerRect.height;
            if(0 > x) x = 0;
            if(0 > y) y = 0;
            if(0 > w) w = 0;
            if(0 > h) h = 0;

            byte[] data = new byte[10];
            data[0] = (byte)0xFF;
            data[1] = (byte)(x>>8); data[2] = (byte)(x&0xFF);
            data[3] = (byte)(y>>8); data[4] = (byte)(y&0xFF);
            data[5] = (byte)(w>>8); data[6] = (byte)(w&0xFF);
            data[7] = (byte)(h>>8); data[8] = (byte)(h&0xFF);
            data[9] = (byte)0xFF;
            writeSerial(data);
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setContentView(R.layout.activity_object_tracker);

        mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.fd_activity_surface_view);
        mTrackImageIv = (ImageView)findViewById(R.id.track_img_view_id);
        mOpenCvCameraView.setCameraPermissionGranted(); //该方法用于判断权限后，自行设置，opencv430版本新改的逻辑
        mOpenCvCameraView.setCvCameraViewListener(this);//监听
//        mOpenCvCameraView.setCameraDistance(1.5f); // 设置焦距
//        mOpenCvCameraView.setMaxFrameSize(640, 480);//设置帧大小
        mOpenCvCameraView.setCameraIndex(cameraindex); //摄像头索引  设置
        mViewContainter = (FrameLayout)findViewById(R.id.view_container);
//        DrawRect drawRect = new DrawRect(this, null);
//        FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(FrameLayout.LayoutParams.WRAP_CONTENT, FrameLayout.LayoutParams.WRAP_CONTENT);
//        params.gravity= Gravity.TOP|Gravity.LEFT;
//        addContentView(drawRect, params);

        serialLooper = new Handler(Looper.getMainLooper());
        detectUsbWithBroadcast();
    }

    @Override
    protected List<? extends CameraBridgeViewBase> getCameraViewList() {
        return Collections.singletonList(mOpenCvCameraView);
    }

    @Override
    public void onCameraViewStarted(int width, int height) {
        mGray = new Mat();
        mRgba = new Mat();
    }

    @Override
    public void onCameraViewStopped() {
        mGray.release();
        mRgba.release();
    }

    @Override
    public Mat onCameraFrame(CameraBridgeViewBase.CvCameraViewFrame inputFrame) {
        mRgba = inputFrame.rgba();//RGBA
        mGray = inputFrame.gray();//单通道灰度图

        if (null == mObjectTracker) {
            mObjectTracker = new ObjectTracker(mRgba) {
                @Override
                public void onCalcBackProject(final Bitmap prob) {
                    ObjectTrackerActivity.this.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            mTrackImageIv.setImageBitmap(prob);
                        }
                    });
                }
            };
        }
        if (null != mTrackWindow) {
            RotatedRect rotatedRect = mObjectTracker.objectTracking(mRgba);
    //            Imgproc.ellipse(mRgba, rotatedRect, OBJECT_RECT_COLOR, 6);
            Rect rect = rotatedRect.boundingRect();
            Imgproc.rectangle(mRgba, rect.tl(), rect.br(), OBJECT_RECT_COLOR, 3);
            Log.i(TAG, "Tracker rect:" + rect);
            mTrackerRect = rect;

            mHandlerUI.post(mUpdateUI);
        }
        return mRgba;
    }

    int xDown = 0;
    int yDown = 0;

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        int cols = mRgba.cols();
        int rows = mRgba.rows();
        int xOffset = (mOpenCvCameraView.getWidth() - cols) / 2;
        int yOffset = (mOpenCvCameraView.getHeight() - rows) / 2;
        Log.i(TAG, "cols:"+cols+" rows:"+rows+" W:"+mOpenCvCameraView.getWidth()+" H:"+mOpenCvCameraView.getHeight());
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                xDown = (int) event.getX() - xOffset;
                yDown = (int) event.getY() - yOffset;
                break;
            case MotionEvent.ACTION_UP:
                int xUp = (int) event.getX() - xOffset;
                int yUp = (int) event.getY() - yOffset;

                // 获取跟踪目标
                mTrackWindow = new Rect(Math.min(xDown, xUp), Math.min(yDown, yUp), Math.abs(xUp - xDown), Math.abs(yUp - yDown));
                // 创建跟踪目标
                Bitmap bitmap = mObjectTracker.createTrackedObject(mRgba, mTrackWindow);
                mTrackImageIv.setImageBitmap(bitmap);
                status("已经选中跟踪目标！");
                break;
            default:
                break;
        }
        return true;
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mOpenCvCameraView.disableView();
        releaseSerial();
        unregisterReceiver(mUsbStateChangeReceiver);
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (mOpenCvCameraView != null)
            mOpenCvCameraView.disableView();
    }

    private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS:{
                    Log.i(TAG, "OpenCV loaded successfully");
                    mOpenCvCameraView.enableView();
                }
                break;
                default:{
                    super.onManagerConnected(status);
                }
                break;
            }
        }
    };

    @Override
    public void onResume()
    {
        super.onResume();
        if (!OpenCVLoader.initDebug()) {
            Log.d(TAG, "Internal OpenCV library not found. Using OpenCV Manager for initialization");
            OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_3_0_0, this, mLoaderCallback);
        } else {
            Log.d(TAG, "OpenCV library found inside package. Using it!");
            mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS);
        }
        serialLooper.post(this::openSerial);
    }


    @Override
    public void onPointerCaptureChanged(boolean hasCapture) {

    }

    private BroadcastReceiver mUsbStateChangeReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            Log.d(TAG, "BroadcastReceiver: " + action);
            if(action.equals(UsbManager.ACTION_USB_DEVICE_ATTACHED)){
                openSerial();
            }else if(action.equals(UsbManager.ACTION_USB_DEVICE_DETACHED)){
                releaseSerial();
            }else if(action.equals(INTENT_ACTION_GRANT_USB)){
                openSerial();
            }
        }
    };

    private void detectUsbWithBroadcast() {
        Log.d(TAG, "listenUsb: register");
        IntentFilter filter = new IntentFilter();
        filter.addAction(UsbManager.ACTION_USB_DEVICE_ATTACHED);
        filter.addAction(UsbManager.ACTION_USB_DEVICE_DETACHED);
        filter.addAction(INTENT_ACTION_GRANT_USB);
        registerReceiver(mUsbStateChangeReceiver, filter);
        Log.d(TAG, "listenUsb: registered");
    }

    private SerialInputOutputManager.Listener usbListener = new SerialInputOutputManager.Listener() {
        @Override
        public void onNewData(byte[] data) {
            serialLooper.post(() -> {
                recvSerial(data);
            });
        }

        @Override
        public void onRunError(Exception e) {

        }
    };

    private void openSerial(){
        // Find all available drivers from attached devices.
        UsbManager usbManager = (UsbManager) getSystemService(Context.USB_SERVICE);
        List<UsbSerialDriver> availableDrivers = UsbSerialProber.getDefaultProber().findAllDrivers(usbManager);
        if (availableDrivers.isEmpty()) {
            return ;
        }

        // Open a connection to the first available driver.
        UsbSerialDriver driver = availableDrivers.get(0);
        UsbDeviceConnection usbConnection = usbManager.openDevice(driver.getDevice());
        if(usbConnection == null && usbPermission == UsbPermission.Unknown && !usbManager.hasPermission(driver.getDevice())) {
            usbPermission = UsbPermission.Requested;
            PendingIntent usbPermissionIntent = PendingIntent.getBroadcast(getApplicationContext(), 0, new Intent(INTENT_ACTION_GRANT_USB), 0);
            usbManager.requestPermission(driver.getDevice(), usbPermissionIntent);
            return ;
        }
        if(usbConnection == null) {
            if (!usbManager.hasPermission(driver.getDevice())) {
                //status("connection failed: permission denied");
            }else {
                status("connection failed: open failed");
            }
            return ;
        }
        usbSerialPort = driver.getPorts().get(0); // Most devices have just one port (port 0)
        try {
            usbSerialPort.open(usbConnection);
            usbSerialPort.setParameters(115200, 8, UsbSerialPort.STOPBITS_1, UsbSerialPort.PARITY_NONE);
            usbIoManager = new SerialInputOutputManager(usbSerialPort, usbListener);
            usbIoManager.start();
            connected = true;
        } catch (IOException e) {
            e.printStackTrace();
            connected = false;
            usbSerialPort = null;
            return ;
        }
    }

    private void writeSerial(byte[] data){
        if(!connected) {
            status("串口未连接！");return;
        }
        //byte[] data = (msg + '\n').getBytes();
        try {
            usbSerialPort.write(data, 2000);
            //Log.d(TAG,"COM>>:"+msg);
        } catch (IOException e) {
            e.printStackTrace();
            status("串口发送出错！");
        }
    }

    private void recvSerial(byte[] data){
        try {
            String msg = new String(data, "UTF-8");
            //Log.d(TAG,"COM<<:"+msg);
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
    }

    private void releaseSerial(){
        connected = false;
        if(usbIoManager != null) {
            usbIoManager.setListener(null);
            usbIoManager.stop();
        }
        usbIoManager = null;
        if(null != usbSerialPort) {
            try {
                usbSerialPort.close();
            } catch (IOException ignored) {
            }
        }
        usbSerialPort = null;
        usbPermission = UsbPermission.Unknown;
    }

    private void status(String status){
        Toast.makeText(getApplicationContext(), status, Toast.LENGTH_SHORT).show();
    }
}