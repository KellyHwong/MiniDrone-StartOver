#ifndef RECEIVER_H_
#define RECEIVER_H_

#ifdef __cplusplus
 extern "C" {
#endif

class Receiver {
 private:
  Stick sitck_throttle; // 油门

 public:
  Stick(float min_duty, float max_duty, 0);
  Stick();
}; // class Stick

#ifdef __cplusplus
}
#endif

#endif
