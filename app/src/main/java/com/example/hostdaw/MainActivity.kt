package com.example.hostdaw

import android.app.Activity
import android.os.Bundle
import android.view.Gravity
import android.widget.Button
import android.widget.LinearLayout
import android.widget.SeekBar
import android.widget.TextView

class MainActivity : Activity() {
    private var running = false

    external fun nativeStart(): Boolean
    external fun nativeStop()
    external fun nativeSetFreq(f: Float)

    companion object {
        init { System.loadLibrary("hostengine") }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val label = TextView(this).apply { text = "440 Hz"; textSize = 24f }
        val button = Button(this).apply { text = "Start" }
        val slider = SeekBar(this).apply { max = 1800; progress = 440 - 200 }

        button.setOnClickListener {
            running = if (running) { nativeStop(); false } else nativeStart()
            button.text = if (running) "Stop" else "Start"
        }
        slider.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(s: SeekBar?, p: Int, fromUser: Boolean) {
                val hz = 200 + p
                label.text = "$hz Hz"
                nativeSetFreq(hz.toFloat())
            }
            override fun onStartTrackingTouch(s: SeekBar?) {}
            override fun onStopTrackingTouch(s: SeekBar?) {}
        })

        setContentView(LinearLayout(this).apply {
            orientation = LinearLayout.VERTICAL
            gravity = Gravity.CENTER
            setPadding(48, 48, 48, 48)
            addView(label); addView(button); addView(slider)
        })
    }

    override fun onPause() {
        super.onPause()
        if (running) { nativeStop(); running = false }
    }
}
