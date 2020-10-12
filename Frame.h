#ifndef FRAME_H
#define FRAME_H

#include <QByteArray>
#include "Color.h"

class Frame {
private:
    //rgb channels ..... then binary channels
    QByteArray data;


public:
    Frame(){}



    Frame(Color *rgbData, int numRgbChannels, QByteArray opacityData, int numOpacityChannels, QByteArray binaryData, int numBinaryChannels){

        for(int i=0; i< numRgbChannels; i++){
            this->data.push_back(rgbData[i].toRgb());
        }

        for(int i=0; i< numOpacityChannels; i++){
            this->data.push_back(opacityData[i]);
        }

        for(int i=0; i< numBinaryChannels; i++){
            this->data.push_back(binaryData[i]);
        }
        assert(this->data.size() == (numRgbChannels * 3) + numBinaryChannels + numOpacityChannels);
    }

    void clone(const Frame &other){
        this->data = QByteArray(other.data);
    }

    QByteArray toByteArray(){
        return this->data;
    }



};

#endif // FRAME_H
