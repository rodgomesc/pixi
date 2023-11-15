#include <Halide.h>

namespace {
    using namespace Halide;

    class YuvToRgba888 : public ::Halide::Generator<YuvToRgba888> {
        static Halide::Expr i16(Halide::Expr e) { return Halide::cast<int16_t>(e); }
        static Halide::Expr u32(Halide::Expr e) { return Halide::cast<uint32_t>(e); }
        static Halide::Expr clamp(Halide::Expr e) {
            return Halide::clamp(e, 0, 255);
        }
    public:
        Input<Buffer<uint8_t, 2>> y_{"y_"};
        Input<Buffer<uint8_t, 2>> u_{"u_"};
        Input<Buffer<uint8_t, 2>> v_{"v_"};

        Output<Buffer<uint32_t, 2>> argb_output_{"argb_output_"};

        void generate() {
            Var x("x"), y("y");

            // Algorithm
            Expr y_val = i16(y_(x, y));
            Expr u_val = i16(u_(x / 2, y / 2)) - 128;
            Expr v_val = i16(v_(x / 2, y / 2)) - 128;

            Expr r = clamp(u32(y_val + 1.370705f * v_val));
            Expr g = clamp(u32(y_val - 0.698001f * v_val - 0.337633f * u_val));
            Expr b = clamp(u32(y_val + 1.732446f * u_val));
            Expr alpha = (u32(255) << 24);
            argb_output_(x, y) = alpha | r << 16 | g << 8 | b;


            Var xi("xi"), yi("yi");
            argb_output_.compute_root()
                            .split(y, y, yi, 64)
                            .split(x, x, xi, 32)
                            .vectorize(xi, natural_vector_size<uint8_t>())
                            .reorder(xi, x, y)
                            .parallel(y);

        }
    };
}  // namespace

HALIDE_REGISTER_GENERATOR(YuvToRgba888, yuv_to_rgba888)

////// The main function for the generator
//int main(int argc, char **argv) {
//    return Halide::Internal::generate_filter_main(argc, argv);
//}
