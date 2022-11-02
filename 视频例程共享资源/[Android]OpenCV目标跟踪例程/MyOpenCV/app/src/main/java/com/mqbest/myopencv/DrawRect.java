package com.mqbest.myopencv;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.view.MotionEvent;
import android.view.View;

public class DrawRect extends View {
    private Paint mPaint = null;
    private int StrokeWidth = 5;
    private Rect rect = new Rect(1000, 270 ,1800, 1070);//手动绘制矩形
    public DrawRect(Context context, Rect rect) {
        super(context);
        mPaint = new Paint();
        if(null != rect)
            this.rect = rect;
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        //canvas.drawARGB(50,255,227,0);//就是用ARGB颜色填充画布
        //设置无锯齿
        mPaint.setAntiAlias(true);
        mPaint.setStyle(Paint.Style.STROKE);
        mPaint.setStrokeWidth(StrokeWidth);
        mPaint.setColor(Color.RED);
        mPaint.setAlpha(100);
        // 绘制绿色矩形
        canvas.drawRect(rect, mPaint);
    }

//    @Override
//    public boolean onTouchEvent(MotionEvent event) {
//        int x = (int)event.getX();
//        int y = (int)event.getY();
//        switch (event.getAction()){
//            case MotionEvent.ACTION_DOWN:
//                rect.right += StrokeWidth;
//                rect.bottom += StrokeWidth;
//                invalidate(rect);
//                rect.left = x;
//                rect.top = y;
//                rect.right =rect.left;
//                rect.bottom = rect.top;
//            case MotionEvent.ACTION_MOVE:
//                Rect old = new Rect(rect.left,rect.top,rect.right+StrokeWidth,rect.bottom+StrokeWidth);
//                rect.right = x;
//                rect.bottom = y;
//                old.union(x,y);
//                invalidate(old);
//                break;
//            case MotionEvent.ACTION_UP:
//                break;
//            default:
//                break;
//        }
//        return  true;//处理了触摸信息，消息不再传递
//    }
    public Rect getRect() {
        return rect;
    }
}
