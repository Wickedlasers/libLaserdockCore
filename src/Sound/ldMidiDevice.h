#ifndef LDMIDIDEVICE_H
#define LDMIDIDEVICE_H

#include "ldSoundStubDevice.h"

class ldMidiInfo;
class ldMidiInput;

class ldMidiDevice : public ldSoundStubDevice
{
    Q_OBJECT

public:
    static void registerMetaType();

    explicit ldMidiDevice(QObject *parent);
    ~ldMidiDevice();
        
    void start(const ldMidiInfo &info);
    void stop() override;

    ldMidiInput *input() const;

private:
    ldMidiInput *m_input;
};



#endif //LDMIDIDEVICE_H

