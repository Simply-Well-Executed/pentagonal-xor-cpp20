// SPDX-License-Identifier: 0BSD
// NiteRex Web9.X native XHTML projection V1 for Windows.
// Original interactive WEB9.X pages are NOT silently replaced or certified.
// C++20 / Win32 WriteFile only, no browser runtime, no network, no STL.
#if !defined(_WIN32)
#error "NITEREX|HOLD|this_native_xhtml_writer_requires_Windows"
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#ifndef NOMINMAX
#define NOMINMAX 1
#endif
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN 1
#endif
#include <windows.h>
#include "niterex_web9x_ninebit.hpp"

namespace {
struct writer {
  HANDLE handle;
  bool valid;
  void put(const char* input) noexcept {
    if (!valid || input == nullptr) { valid=false; return; }
    const char* last=input;
    while (*last != '\0') ++last;
    while (input != last) {
      const auto remaining=static_cast<unsigned long>(last-input);
      DWORD written=0;
      if (!WriteFile(handle,input,remaining,&written,nullptr) || written==0U) {
        valid=false;return;
      }
      input+=written;
    }
  }
  void decimal(unsigned number) noexcept {
    char buffer[11]{};
    unsigned index=10U;
    do {
      buffer[--index]=static_cast<char>('0'+number%10U);
      number/=10U;
    } while (number!=0U && index>0U);
    put(buffer+index);
  }
};

void table_entry(writer& o, unsigned slot, const char* name,
                 const char* description) noexcept {
  o.put("<tr><th scope=\"row\">");
  o.decimal(slot);
  o.put("</th><td>");
  o.put(name);
  o.put("</td><td>");
  o.put(description);
  o.put("</td></tr>\n");
}
}

int main() {
  if (!swe::niterex::self_test()) return 3;
  writer o{GetStdHandle(STD_OUTPUT_HANDLE),true};
  if (o.handle==INVALID_HANDLE_VALUE || o.handle==nullptr) return 2;
  o.put("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  o.put("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n");
  o.put("<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n");
  o.put("<head><title>NiteRex | Web9.X | Native C++20 Witness</title>\n");
  o.put("<meta http-equiv=\"Content-Type\" content=\"application/xhtml+xml; charset=UTF-8\" />\n");
  o.put("<style type=\"text/css\"><![CDATA[\n");
  o.put("html{background:#081827;color:#e4f7ff;font-family:Arial,sans-serif}");
  o.put("body{max-width:930px;margin:3em auto;padding:1em}");
  o.put("h1{color:#76e8d0}h2{color:#a7d7ee}");
  o.put("table{border-collapse:collapse;width:100%;margin:1em 0}");
  o.put("th,td{border:1px solid #4a7085;padding:.6em;text-align:left}");
  o.put("th{background:#14344b}.note{border-left:4px solid #edbe89;padding:1em;background:#183246}");
  o.put("]]></style></head>\n<body>\n");
  o.put("<h1>NiteRex / Web9.X</h1>\n");
  o.put("<p>Native C++20 nine-bit operation witness. Independent 0BSD implementation.</p>\n");
  o.put("<p class=\"note\">Not the original interactive browser app, a cryptographic signature, a certificate verification, a Web9.X conformance certification, or an OpenBSD binary. All certificate slots remain unverified.</p>\n");
  o.put("<h2>NiteRex nine quests</h2>\n");
  o.put("<p>Operands A = 13 and B = 10, width = 9 bits, result range = 0 to 511.</p>\n");
  o.put("<table summary=\"Nine computed integer operations\">");
  o.put("<caption>Nine operation witnesses</caption>");
  o.put("<thead><tr><th scope=\"col\">Quest</th><th scope=\"col\">Operation</th><th scope=\"col\">Result</th></tr></thead><tbody>\n");
  unsigned checksum=0U;
  for (unsigned i=0U;i<9U;++i) {
    const auto op=swe::niterex::nine_quests[i];
    const auto answer=swe::niterex::evaluate(op,13U,10U);
    if (!answer.valid) return 4;
    o.put("<tr><th scope=\"row\">");
    o.decimal(i+1U);
    o.put("</th><td>");
    o.put(swe::niterex::name(op));
    o.put("</td><td>");
    o.decimal(answer.value);
    o.put("</td></tr>\n");
    checksum+=answer.value;
  }
  o.put("</tbody></table><p>Reference checksum: ");
  o.decimal(checksum);
  o.put("</p>\n");
  o.put("<h2>Original nine-slot source layout</h2>\n");
  o.put("<table summary=\"Original Web9.X slots and current scope\">");
  o.put("<caption>Slots are distinct; missing authority is not synthesized</caption>");
  o.put("<thead><tr><th scope=\"col\">Slot</th><th scope=\"col\">Field</th><th scope=\"col\">Native V1 handling</th></tr></thead><tbody>\n");
  table_entry(o,1U,"CSS","XHTML presentation only; not independently signed");
  table_entry(o,2U,"HTML / XHTML","Native emitted XHTML 1.0 Strict text");
  table_entry(o,3U,"JavaScript","Not executed by the native renderer");
  table_entry(o,4U,"Java","Not executed by the native renderer");
  table_entry(o,5U,"CSV","Not imported or trusted by this witness");
  table_entry(o,6U,"SVN","No repository revision claimed");
  table_entry(o,7U,"Root CA","Not supplied or verified");
  table_entry(o,8U,"Intermediate CA","Not supplied or verified");
  table_entry(o,9U,"Subject certificate","Not supplied; no signing performed");
  o.put("</tbody></table>\n");
  o.put("<p>Provenance: derived native calculation and static XML projection only. The separate nine-slot Ed25519 experiment remains an unpromoted exploratory protocol.</p>\n");
  o.put("</body></html>\n");
  if (!o.valid || checksum!=829U) return 5;
  return 0;
}
