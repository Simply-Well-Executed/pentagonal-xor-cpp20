// SPDX-License-Identifier: 0BSD
#include "../shared/ninza_ping_scheduler.hpp"
#include "../shared/sigma_penta_ping.hpp"
#include <array>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <stdexcept>
namespace s = swe::ninza::scheduler;
namespace p = swe::sigma_penta_ping;
int main() {
    constexpr std::size_t n = 9;
    const std::array<std::size_t,n> order{8,1,6,3,4,5,2,7,0};
    using Block=std::array<unsigned char,p::ping_bytes>;
    std::array<Block,n> core{}, serial{}, parallel{}, restored{};
    for(std::size_t id=0;id<n;++id)
        for(std::size_t i=0;i<p::ping_bytes;++i)
            core[id][i]=static_cast<unsigned char>(id*p::ping_bytes+i);
    const auto twirl=[&](std::size_t id, auto& dest) {
        for(unsigned c=0;c<p::stripe_bytes;++c)
            for(unsigned j=0;j<p::loops;++j)
                dest[id][c*p::loops+j]=core[id][p::lane(c,j)*p::stripe_bytes+c];
    };
    std::array<std::atomic<unsigned>,n> visits{};
    const auto a=s::execute(n,order,1,[&](std::size_t id) {
        twirl(id,serial); return true;
    });
    const auto b=s::execute(n,order,4,[&](std::size_t id) {
        ++visits[id]; twirl(id,parallel); return true;
    });
    const auto inverse=s::execute(n,order,4,[&](std::size_t id) {
        for(unsigned c=0;c<p::stripe_bytes;++c)
            for(unsigned j=0;j<p::loops;++j)
                restored[id][p::lane(c,j)*p::stripe_bytes+c]=parallel[id][c*p::loops+j];
        return true;
    });
    if(a.verdict!=s::Verdict::complete || b.verdict!=s::Verdict::complete ||
       inverse.verdict!=s::Verdict::complete || a.workers!=1 || b.workers!=4 ||
       serial!=parallel || restored!=core) return 1;
    for(auto& v:visits) if(v.load()!=1) return 2;
    std::array<std::size_t,n> claimed{};
    std::size_t pos=0;
    const auto c=s::execute(n,order,1,[&](std::size_t id) {
        claimed[pos++]=id; return true;
    });
    if(c.verdict!=s::Verdict::complete || claimed!=order) return 3;
    auto duplicate=order; duplicate[0]=duplicate[1];
    unsigned effects=0;
    const auto deny=[&](std::size_t) { ++effects; return true; };
    if(s::execute(n,duplicate,1,deny).verdict!=s::Verdict::hold ||
       s::execute(n,{},1,deny).verdict!=s::Verdict::hold ||
       s::execute(n,order,0,deny).verdict!=s::Verdict::hold ||
       s::execute(n,order,65,deny).verdict!=s::Verdict::hold || effects!=0) return 4;
    const auto failed=s::execute(n,order,3,[](std::size_t id) {
        if(id==2) throw std::runtime_error("fixture");
        return id!=5;
    });
    if(failed.verdict!=s::Verdict::hold || failed.jobs[2]!=s::State::hold ||
       failed.jobs[5]!=s::State::hold || failed.jobs[0]!=s::State::pass) return 5;
    const std::array<std::size_t,2> pair{1,0};
    std::atomic<unsigned> entered{0};
    const auto overlap=s::execute(2,pair,2,[&](std::size_t) {
        entered.fetch_add(1);
        const auto until=std::chrono::steady_clock::now()+std::chrono::seconds(3);
        while(entered.load()!=2 && std::chrono::steady_clock::now()<until)
            std::this_thread::yield();
        return entered.load()==2;
    });
    if(overlap.verdict!=s::Verdict::complete) return 6;
    std::puts("NINZA-PING-EXECUTOR-v1: PING inverse, serial/parallel, exact-once, order, HOLD and overlap PASS");
    return 0;
}
