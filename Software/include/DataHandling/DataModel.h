#ifndef DATA_MODEL_H
#define DATA_MODEL_H

#include <Arduino.h>

namespace DataModel
{
    // ############# AIN #################
    struct ainSettings {
        uint8_t gain = 0;
        uint8_t mode = 0;
        char adc1_value[10];
        char adc2_value[10];
        char adc3_value[10];
        char adc4_value[10];
    };

    extern struct ainSettings ain;


    // ############# Relais #################
    struct relaisSettings {
        bool enable1 = false;
        bool enable2 = false;
    };

    extern struct relaisSettings relais;


    // ############# Power supplies #################
    struct inaValues {
        char voltage[10];	// xxxx.xxV\n
        char current[10];	// xxxx.xxmA\n
        char power[10];		// xxxx.xxmW\n
    };

    struct vccSettings {
        struct inaValues ina;
        float value = 0;
        bool enable = false;
    };

    extern struct vccSettings vcc;
    extern struct vccSettings vdc;


    // ############# DOUT #################
    // ToDo: add INA219
    struct doutSettings {
        uint8_t value = 0;
    };

    extern struct doutSettings dout1;
    extern struct doutSettings dout2;
    extern struct doutSettings dout3;
    extern struct doutSettings dout4;


    // ############# FOUT #################
    struct foutSettings {
        char frequency[50]="1000";
        float offset = 0.0;
        uint8_t mode = 0;
        uint8_t multiplier = 0;
    };

    extern struct foutSettings fout;


    // ####### Battery simulation ########
    struct batSettings {
        float value = 0.0;
        struct inaValues ina;
    };

    extern struct batSettings bat;


    // ########### Level shift ###########
    struct lshiftSettings {
        bool enable1 = false;
        bool enable2 = false;
        bool enable3 = false;
        bool enable4 = false;
    };

    extern struct lshiftSettings lshift;
    
};

#endif // DATA_MODEL_H