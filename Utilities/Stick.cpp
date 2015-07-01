#define STD_MIN 5.0
#define STD_MAX 10.0
#define STD_MID 7.5
#define STD_INTERVAL 5.0

Stick::Stick() {}

Stick(float min_duty, float max_duty, 0) {}

Stick::float data_convert(float data) {
  switch (mode_) {
    case (NEGATIVE_LOGIC): return data_invert();
  }
}

Stick::float data_invert(float data) {

}

Stick::data_standardize() {
  switch (mode_) {
    case (NEGATIVE_LOGIC): {
      std_duty_ =
    }
  }
}
