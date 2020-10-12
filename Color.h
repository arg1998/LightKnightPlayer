#ifndef COLOR_H
#define COLOR_H

#include <QByteArray>
#include <QString>
#include <QColor>

typedef unsigned char uchar;

class Color {
private:

    QByteArray colors_array;

public:
    Color(){
        this->colors_array.resize(3);
        this->colors_array[0] = 0;
        this->colors_array[1] = 0;
        this->colors_array[2] = 0;
    }

    Color(QString colorCode){
        QColor _c(colorCode);
        this->colors_array.resize(3);
        this->colors_array[0] = _c.red();
        this->colors_array[1] = _c.green();
        this->colors_array[2] = _c.blue();
    }

    Color(uchar r,uchar g, uchar b){
        this->colors_array.resize(3);
        this->colors_array[0] = r;
        this->colors_array[1] = g;
        this->colors_array[2] = b;
    }

    ~Color(){
        colors_array.clear();
    }

    QByteArray toRgb(){
        return this->colors_array;
    }
};

const Color LED_BLACK_COLOR = Color(0, 0, 0);
const Color LED_WHITE_COLOR = Color(255, 255, 255);

const unsigned char LED_OFF  = 0x00;
const unsigned char LED_ON = 0xFF;

#endif // COLOR_H
