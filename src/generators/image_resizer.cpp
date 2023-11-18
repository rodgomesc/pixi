#include "Halide.h"

using namespace Halide;

class ImageResizer : public Halide::Generator<ImageResizer> {
public:
    Input<Buffer<uint8_t>> input{"input", 3};
    Input<int> output_width{"output_width"};
    Input<int> output_height{"output_height"};

    Output<Buffer<uint8_t>> output{"output", 3};

    Func calculateSourceCoordinates() {
        Var x("x"), y("y");
        Func coordinates("coordinates");

        Expr scale_x = cast<float>(input.width()) / output_width;
        Expr scale_y = cast<float>(input.height()) / output_height;

        Expr source_x = min(input.width() - 1, max(0, (x + 0.5f) * scale_x - 0.5f));
        Expr source_y = min(input.height() - 1, max(0, (y + 0.5f) * scale_y - 0.5f));

        coordinates(x, y) = {source_x, source_y};

        return coordinates;
    }

    Func bilinearInterpolate(Func coordinates) {
        Func interpolated("interpolated");
        Var x("x"), y("y"), c("c");

        Func clamped = BoundaryConditions::repeat_edge(input);
        Expr ix = cast<int>(coordinates(x, y)[0]);
        Expr iy = cast<int>(coordinates(x, y)[1]);
        Expr fx = coordinates(x, y)[0] - ix;
        Expr fy = coordinates(x, y)[1] - iy;

        Expr tl = cast<float>(clamped(ix, iy, c));
        Expr tr = cast<float>(clamped(ix + 1, iy, c));
        Expr bl = cast<float>(clamped(ix, iy + 1, c));
        Expr br = cast<float>(clamped(ix + 1, iy + 1, c));

        interpolated(x, y, c) = lerp(lerp(tl, tr, fx), lerp(bl, br, fx), fy);

        return interpolated;
    }

    void generate() {
        Var x("x"), y("y"), c("c");

        Func coordinates = calculateSourceCoordinates();
        Func interpolated = bilinearInterpolate(coordinates);

        output(x, y, c) = cast<uint8_t>(clamp(interpolated(x, y, c), 0.0f, 255.0f));
        Var xi("xi"), yi("yi"), xo("xo"), yo("yo");

        output.tile(x, y, xo, yo, xi, yi, 128, 32)
        .vectorize(xi, 32)
        .parallel(yo);

    }
};

HALIDE_REGISTER_GENERATOR(ImageResizer, image_resizer)
