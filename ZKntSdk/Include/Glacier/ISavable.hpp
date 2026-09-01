#pragma once

class ISavable {
  public:
    virtual ~ISavable() = 0;
};

class ISavableEntity : public IComponentInterface {};
