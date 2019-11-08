package com.sensing.core.coreviewer;

public class RingBuffer {

    private int size;
    private int counter = 0;
    private Double[] ringbuffer;

    public RingBuffer(int size){
        this.size = size;
        ringbuffer = new Double[size];

        for(int i = 0; i<size; i++){
            ringbuffer[i] = 0.0;
        }
    }

    public void put(double value){
        if(counter < size){
            ringbuffer[counter] = value;
            counter++;
        }else{
            counter = 0;
            ringbuffer[counter] = value;
            counter++;
        }
    }

    public int getSize(){
        return ringbuffer.length;
    }

    public Double getCurrentValue(){
        return ringbuffer[counter-1];
    }

    public Double[] getBuffer(){
        return ringbuffer;
    }

    public Double getAverage(){
        Double average = 0.0;
        for(int i = 0; i<size; i++){
            average += ringbuffer[i];
        }
        return average = average/size;
    }
}
