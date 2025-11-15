#ifndef FETCH_GUARD_H
#define FETCH_GUARD_H

class FetchGuard {
public:
  FetchGuard(bool& flag) : flag_(flag) { flag_ = true; }
  ~FetchGuard() { flag_ = false; }
private:
  bool& flag_;
};

#endif
