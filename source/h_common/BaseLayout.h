#pragma once

class BaseLayout {
public:
    virtual bool InitGL() = 0;
    virtual void DeInitGL() = 0;
};
