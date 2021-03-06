#ifndef SUBJECT_H
#define SUBJECT_H
#include <vector>
#include "state.h"
#include "observer.h"

class Subject {
  std::vector<Observer*> observers;
  State state;
 protected:
  void setState(State newS);
 public:
  void attach(Observer *o);
  void clear();  
  void notifyObservers();
  State getState() const;
};

#endif
