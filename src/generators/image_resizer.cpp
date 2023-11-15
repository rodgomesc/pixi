#include "Halide.h"

using namespace Halide;

class ImageResizer : public Halide::Generator<ImageResizer> {
public:
    Input<Buffer<uint8_t>> input{"input", 3};
    Input<int> output_width{"output_width"};
    Input<int> output_height{"output_height"};

    Output<Buffer<uint8_t>> output{"output", 3};

    void generate() {
        Var x("x"), y("y"), c("c");

        Expr scale_x = cast<float>(input.width()) / cast<float>(output_width);
        Expr scale_y = cast<float>(input.height()) / cast<float>(output_height);

        Expr source_x = min(input.width() - 1, max(0, (x + 0.5f) * scale_x - 0.5f));
        Expr source_y = min(input.height() - 1, max(0, (y + 0.5f) * scale_y - 0.5f));

        Expr ix = cast<int>(source_x);
        Expr iy = cast<int>(source_y);

        Expr fx = source_x - ix;
        Expr fy = source_y - iy;

        Func clamped = BoundaryConditions::repeat_edge(input);
        Expr tl = cast<float>(clamped(ix, iy, c));
        Expr tr = cast<float>(clamped(ix + 1, iy, c));
        Expr bl = cast<float>(clamped(ix, iy + 1, c));
        Expr br = cast<float>(clamped(ix + 1, iy + 1, c));

        Expr interpolated = lerp(lerp(tl, tr, fx), lerp(bl, br, fx), fy);

        output(x, y, c) = cast<uint8_t>(clamp(interpolated, 0.0f, 255.0f));



        Var xi("xi"), yi("yi");
        output.compute_root()
                .split(y, y, yi, 64)
                .split(x, x, xi, 32)
                .vectorize(xi, natural_vector_size<uint8_t>())
                .reorder(xi, x, y)
                .parallel(y);

    }

};

HALIDE_REGISTER_GENERATOR(ImageResizer, image_resizer)
