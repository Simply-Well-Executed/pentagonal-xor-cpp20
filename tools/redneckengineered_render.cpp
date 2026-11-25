// SPDX-License-Identifier: 0BSD
// Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824
// Native CPU renderer: writes six real binary PPM cube-face image files.
// Usage: redneckengineered_render.exe <existing-output-directory>
// Output is exploratory; undefined DIFZ/final publishing is always HOLD.
#include "../shared/redneckengineered_native_render.hpp"
#include <cstdio>
#include <string>
namespace graphics=swe::redneckengineered::native_render;
namespace u=swe::u_circle_fit;
[[nodiscard]] bool emit(const std::string& filename,
                        const graphics::Bitmap& bitmap) {
    FILE* file=nullptr;
    if(fopen_s(&file,filename.c_str(),"wb")!=0 || !file)return false;
    constexpr char header[]="P6\n# C5824 PENNYWORTH\n64 64\n255\n";
    bool okay=std::fwrite(header,1u,sizeof(header)-1u,file)==
              sizeof(header)-1u;
    for(const auto& pixel:bitmap) {
        const unsigned char rgb[3]{pixel.r,pixel.g,pixel.b};
        if(std::fwrite(rgb,1u,3u,file)!=3u){okay=false;break;}
    }
    if(std::fclose(file)!=0)okay=false;
    return okay;
}
[[nodiscard]] bool self_test() {
    auto result=graphics::render(graphics::established_model);
    if(!graphics::check(result) ||
       !graphics::equal(result.baked,result.cooked) ||
       !graphics::has_color(result.cubemap.faces[4u]) ||
       result.difz_verified || result.release_authorized)return false;
    auto altered=graphics::established_model;
    altered.circle=u::HEIGHT_CIRCLE;
    const auto negative=graphics::render(altered);
    if(negative.geometry_nice ||
       graphics::has_color(negative.baked) ||
       negative.release_authorized)return false;
    for(unsigned face=0;face<graphics::FACE_COUNT;++face) {
        // Every source face ray is determined by its own projection.
        const auto ray=graphics::direction(face,17u,29u);
        if(!graphics::finite(ray.x)||!graphics::finite(ray.y)||
           !graphics::finite(ray.z))return false;
    }
    return true;
}
int main(int argc,char** argv) {
    if(!self_test()) {
        std::fputs("REDNECKENGINEERED|RENDER_SELF_TEST=FAIL\n",stderr);
        return 2;
    }
    if(argc!=2 || !argv[1] || !argv[1][0]) {
        std::fputs("Usage: redneckengineered_render <existing-output-directory>\n",stderr);
        return 3;
    }
    const auto result=graphics::render(graphics::established_model);
    if(!graphics::check(result))return 4;
    const std::string output=argv[1];
    const std::string separator=output.back()=='\\'||output.back()=='/'?"":"\\";
    const std::string root=output+separator;
    if(!emit(root+"bake.ppm",result.baked) ||
       !emit(root+"cook.ppm",result.cooked))return 5;
    constexpr const char* labels[]{"posx","negx","posy","negy","posz","negz"};
    for(unsigned face=0;face<graphics::FACE_COUNT;++face)
        if(!emit(root+"cubemap_"+labels[face]+".ppm",
                 result.cubemap.faces[face]))return 6;
    std::puts("REDNECKENGINEERED|MODEL_BAKE_BITMAP_COOK_CUBEMAP=OUTPUT_WRITTEN|FACE_COUNT=6|DIFZ=HOLD|PUBLISH=HOLD");
    return 0;
}
