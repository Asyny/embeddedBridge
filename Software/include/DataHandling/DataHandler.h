#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H


class DataHandler {

public:
    DataHandler(void); 
    virtual ~DataHandler(void);

    void process(void);

private:
    void updateRelais(void);
    void updateLevelShift(void);
    void updateAdcValues(void);
    void updatePsuMonitoringValues(void);
    void updatePsu(void);
    void updateDigitalOutputs(void);
    void updateFrequencyOutput(void);
    void updateBatterySimulation(void);
    void updateAnalogInputGain(void);
};

#endif // DATA_HANDLER_H