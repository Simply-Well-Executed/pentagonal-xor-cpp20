// SPDX-License-Identifier: 0BSD
// DOVIA-STAR-001: fixed graph model. No std headers or dynamic allocation.
// YANG uses the SPF shortest-path principle of ORIGINAL RFC 2328 (April 1998),
// Section 16.1. Not an OSPF implementation; NO errata are incorporated.
// YIN is user-proposed RUSPT security policy plus guarded A* search.
// Ethernet STP itself is IEEE 802.1D/802.1Q, NOT the SPF algorithm.
#pragma once
namespace dovia {
// VIEW-002: one logical observation source. The exact half-apertures
// are sqrt(15) degrees in front and sqrt(6) degrees in rear.
// Integer centidegree input is a stated measurement resolution,
// NOT a rounded representation of either irrational root.
// Compare (centidegrees)^2 <= (radicand)*10000 exactly.
enum class ViewSide {front,rear};
struct SingleView {
  unsigned observer_count;
  int front_half_radicand,rear_half_radicand;
};
inline constexpr SingleView single_view{1u,15,6};
constexpr bool valid_single_view(const SingleView& v) noexcept {
  return v.observer_count==1u &&
         v.front_half_radicand==15 &&
         v.rear_half_radicand==6;
}
constexpr bool inside_single_view(const SingleView& v,
                                  ViewSide side,int signed_offset_centidegrees) noexcept {
  if(!valid_single_view(v)) return false;
  // All allowed angles are inside [-4,+4] degrees; bound before square.
  if(signed_offset_centidegrees < -400 ||
     signed_offset_centidegrees > 400) return false;
  const long long offset=signed_offset_centidegrees;
  const long long square=offset*offset;
  if(side==ViewSide::front)
    return square <= 15ll*10000ll;
  if(side==ViewSide::rear)
    return square <= 6ll*10000ll;
  return false;
}
static_assert(valid_single_view(single_view));
static_assert(inside_single_view(single_view,ViewSide::front,0));
static_assert(inside_single_view(single_view,ViewSide::front,-387));
static_assert(inside_single_view(single_view,ViewSide::front,387));
static_assert(!inside_single_view(single_view,ViewSide::front,388));
static_assert(!inside_single_view(single_view,ViewSide::front,-388));
static_assert(inside_single_view(single_view,ViewSide::rear,-244));
static_assert(inside_single_view(single_view,ViewSide::rear,244));
static_assert(!inside_single_view(single_view,ViewSide::rear,245));
static_assert(!inside_single_view(single_view,ViewSide::rear,-245));
static_assert(!valid_single_view(SingleView{2u,15,6}));
static_assert(!valid_single_view(SingleView{1u,16,6}));
static_assert(!valid_single_view(SingleView{1u,15,7}));
constexpr int nodes=5,root=0,goal=4,infinity=999999,port_count=10;
struct Edge {int source,target,weight;};
inline constexpr Edge graph[]={
  {0,1,2},{0,2,4},{1,3,3},{1,4,5},{2,4,2},{3,4,1}
};
constexpr int edge_cost(int a,int b) noexcept {
  for(const Edge& e:graph)
    if(e.source==a && e.target==b) return e.weight;
  return infinity;
}
struct Tree {int cost[nodes];int parent[nodes];bool settled[nodes];};
// YANG: deterministic, nonnegative-cost Dijkstra/SPF tree, rooted at 0.
// RFC 2328's LSA, adjacencies, ECMP and next-hop machinery are NOT modeled.
constexpr Tree original_rfc_spf() noexcept {
  Tree tree{};
  for(int i=0;i<nodes;++i) {
    tree.cost[i]=infinity;tree.parent[i]=-1;tree.settled[i]=false;
  }
  tree.cost[root]=0;
  for(int round=0;round<nodes;++round) {
    int u=-1;
    for(int i=0;i<nodes;++i)
      if(!tree.settled[i] &&
         (u<0 || tree.cost[i]<tree.cost[u] ||
          (tree.cost[i]==tree.cost[u] && i<u))) u=i;
    if(u<0 || tree.cost[u]==infinity) break;
    tree.settled[u]=true;
    for(const Edge& e:graph) {
      if(e.source!=u || tree.settled[e.target] ||
         e.weight<0 || tree.cost[u]>infinity-e.weight) continue;
      const int next=tree.cost[u]+e.weight;
      if(next<tree.cost[e.target]) {
        tree.cost[e.target]=next;tree.parent[e.target]=u;
      }
    }
  }
  return tree;
}
constexpr bool tree_proof(const Tree& t) noexcept {
  if(t.cost[root]!=0 || t.parent[root]!=-1) return false;
  for(int v=0;v<nodes;++v) {
    if(!t.settled[v] || t.cost[v]>=infinity) return false;
    if(v!=root) {
      const int p=t.parent[v],w=edge_cost(p,v);
      if(p<0 || p>=nodes || w>=infinity || w<0 ||
         t.cost[p]>infinity-w || t.cost[p]+w!=t.cost[v]) return false;
      int a=v;
      for(int step=0;step<nodes && a!=root;++step) {
        a=t.parent[a];
        if(a<0 || a>=nodes) return false;
      }
      if(a!=root) return false;
    }
  }
  for(const Edge& e:graph)
    if(e.source<0 || e.source>=nodes || e.target<0 ||
       e.target>=nodes || e.weight<=0 ||
       t.cost[e.source]>infinity-e.weight ||
       t.cost[e.target]>t.cost[e.source]+e.weight) return false;
  return true;
}
inline constexpr Tree yang=original_rfc_spf();
static_assert(tree_proof(yang));
static_assert(yang.cost[0]==0 && yang.cost[1]==2 && yang.cost[2]==4 &&
              yang.cost[3]==5 && yang.cost[4]==6);
static_assert(yang.parent[0]==-1 && yang.parent[1]==0 &&
              yang.parent[2]==0 && yang.parent[3]==1 &&
              yang.parent[4]==2);

enum class Role {designated,root_uplink,host_edge};
enum class PortState {forwarding,root_inconsistent,error_disabled};
struct Bridge {unsigned priority;unsigned long long stable_id;};
struct Port {
  int owner,peer;
  Role role;
  bool root_guard,portfast,bpdu_guard,bpdu_received,superior_bpdu;
};
struct Channel {
  unsigned group,member,speed_mbps;
  int owner,peer,vlan_first,vlan_last;
  bool trunk;
};
struct Plan {
  int vlan_first,vlan_last;
  bool etherchannel_guard_misconfig;
  Bridge bridges[nodes];
  Port ports[port_count];
  Channel members[2];
};
// VLAN is 1..4094 on Catalyst 3650. The example "1-4096" fails closed.
constexpr bool valid_vlans(int first,int last) noexcept {
  return first>=1 && last<=4094 && first<=last;
}
constexpr int elected_root(const Plan& p) noexcept {
  int best=0;
  for(int i=1;i<nodes;++i)
    if(p.bridges[i].priority<p.bridges[best].priority ||
       (p.bridges[i].priority==p.bridges[best].priority &&
        p.bridges[i].stable_id<p.bridges[best].stable_id)) best=i;
  return best;
}
constexpr PortState port_state(const Port& port) noexcept {
  if(port.role==Role::host_edge && port.bpdu_guard &&
     port.bpdu_received) return PortState::error_disabled;
  if(port.role==Role::designated && port.root_guard &&
     port.superior_bpdu) return PortState::root_inconsistent;
  return PortState::forwarding;
}
constexpr bool port_configuration(const Plan& p) noexcept {
  int host_count=0;
  for(const Port& port:p.ports) {
    if(port.owner<0 || port.owner>=nodes) return false;
    if(port.role==Role::host_edge) {
      ++host_count;
      if(port.peer!=-1 || !port.portfast || !port.bpdu_guard ||
         port.root_guard) return false;
    } else {
      if(port.peer<0 || port.peer>=nodes || port.peer==port.owner ||
         port.portfast || port.bpdu_guard) return false;
      if(port.role==Role::designated) {
        // Protect all chosen designated downstream ports, not root uplinks.
        if(!port.root_guard || yang.parent[port.peer]!=port.owner)
          return false;
      } else if(port.role==Role::root_uplink) {
        if(port.root_guard || port.owner==root ||
           yang.parent[port.owner]!=port.peer) return false;
      } else return false;
    }
  }
  if(host_count!=2) return false;
  for(int v=1;v<nodes;++v) {
    int downstream=0,upstream=0;
    for(const Port& pport:p.ports) {
      if(pport.role==Role::designated && pport.owner==yang.parent[v] &&
         pport.peer==v) ++downstream;
      if(pport.role==Role::root_uplink && pport.owner==v &&
         pport.peer==yang.parent[v]) ++upstream;
    }
    if(downstream!=1 || upstream!=1) return false;
  }
  return true;
}
constexpr bool channel_configuration(const Plan& p) noexcept {
  if(!p.etherchannel_guard_misconfig) return false;
  const Channel& a=p.members[0];
  const Channel& b=p.members[1];
  return a.group>0u && a.group==b.group &&
    a.member>0u && b.member>0u && a.member!=b.member &&
    a.speed_mbps>0u && a.speed_mbps==b.speed_mbps &&
    a.owner==root && b.owner==root && a.peer==1 && b.peer==1 &&
    a.trunk && b.trunk &&
    a.vlan_first==p.vlan_first && b.vlan_first==p.vlan_first &&
    a.vlan_last==p.vlan_last && b.vlan_last==p.vlan_last;
}
constexpr bool policy_compliant(const Plan& p) noexcept {
  if(!valid_vlans(p.vlan_first,p.vlan_last) ||
     p.vlan_first!=1 || p.vlan_last!=4094 ||
     elected_root(p)!=root || !tree_proof(yang) ||
     !port_configuration(p) || !channel_configuration(p)) return false;
  for(int i=0;i<nodes;++i) {
    if(p.bridges[i].priority>61440u ||
       p.bridges[i].priority%4096u!=0u ||
       p.bridges[i].stable_id==0ull) return false;
    for(int j=0;j<i;++j)
      if(p.bridges[i].stable_id==p.bridges[j].stable_id) return false;
  }
  return true;
}
constexpr bool operational(const Plan& p) noexcept {
  for(const Port& port:p.ports)
    if(port_state(port)!=PortState::forwarding) return false;
  return true;
}
constexpr bool forwarding_edge(const Plan& p,int from,int to) noexcept {
  if(to<=root || to>=nodes || yang.parent[to]!=from) return false;
  bool down=false,up=false;
  for(const Port& port:p.ports) {
    if(port.owner==from && port.peer==to &&
       port.role==Role::designated && port.root_guard &&
       port_state(port)==PortState::forwarding) down=true;
    if(port.owner==to && port.peer==from &&
       port.role==Role::root_uplink && !port.root_guard &&
       port_state(port)==PortState::forwarding) up=true;
  }
  return down && up;
}
// Root primary is a Cisco configuration macro, not a fixed IEEE priority.
// 24576 is an illustrative, explicitly modeled value, NOT an emulated
// result of running the dynamic IOS "root primary" command.
inline constexpr Plan baseline={
  1,4094,true,
  {{24576u,1ull},{32768u,2ull},{32768u,3ull},
   {32768u,4ull},{32768u,5ull}},
  {
    {0,1,Role::designated,true,false,false,false,false},
    {1,0,Role::root_uplink,false,false,false,false,false},
    {0,2,Role::designated,true,false,false,false,false},
    {2,0,Role::root_uplink,false,false,false,false,false},
    {1,3,Role::designated,true,false,false,false,false},
    {3,1,Role::root_uplink,false,false,false,false,false},
    {2,4,Role::designated,true,false,false,false,false},
    {4,2,Role::root_uplink,false,false,false,false,false},
    {0,-1,Role::host_edge,false,true,true,false,false},
    {4,-1,Role::host_edge,false,true,true,false,false}
  },
  {{7u,1u,1000u,0,1,1,4094,true},
   {7u,2u,1000u,0,1,1,4094,true}}
};
static_assert(policy_compliant(baseline));
static_assert(elected_root(baseline)==root);
static_assert(!valid_vlans(1,4096));
static_assert(!forwarding_edge(baseline,1,4)); // non-tree edge is blocked
static_assert(forwarding_edge(baseline,0,2));

constexpr Plan bad_vlan() noexcept {
  Plan p=baseline;p.vlan_last=4096;return p;
}
constexpr Plan stolen_root() noexcept {
  Plan p=baseline;p.bridges[1].priority=0;return p;
}
constexpr Plan unguarded_downstream() noexcept {
  Plan p=baseline;p.ports[0].root_guard=false;return p;
}
constexpr Plan bad_channel() noexcept {
  Plan p=baseline;p.members[1].speed_mbps=100;return p;
}
constexpr Plan edge_bpdu() noexcept {
  Plan p=baseline;p.ports[8].bpdu_received=true;return p;
}
constexpr Plan superior_bpdu() noexcept {
  Plan p=baseline;p.ports[2].superior_bpdu=true;return p;
}
static_assert(!policy_compliant(bad_vlan()));
static_assert(!policy_compliant(stolen_root()));
static_assert(!policy_compliant(unguarded_downstream()));
static_assert(!policy_compliant(bad_channel()));
static_assert(policy_compliant(edge_bpdu()) && !operational(edge_bpdu()));
static_assert(policy_compliant(superior_bpdu()) && !operational(superior_bpdu()));
static_assert(port_state(superior_bpdu().ports[2])==
              PortState::root_inconsistent);
static_assert(port_state(edge_bpdu().ports[8])==
              PortState::error_disabled);

constexpr int turn45(int current,int turns) noexcept {
  // Discrete octants are EXACT; no floating point and no invented geometry.
  return ((current+turns)%8+8)%8;
}
static_assert(turn45(0,3)==3); // 135 degrees
static_assert(turn45(turn45(0,3),1)==4); // 180 degrees
static_assert(turn45(4,4)==0); // inverse half-turn

constexpr int h[nodes]={4,3,2,1,0};
constexpr bool consistent_heuristic() noexcept {
  if(h[goal]!=0) return false;
  for(const Edge& e:graph)
    if(h[e.source]>e.weight+h[e.target]) return false;
  return true;
}
static_assert(consistent_heuristic());
struct Candidate {int node,g,f,depth,path[nodes];};
constexpr bool earlier(const Candidate& a,const Candidate& b) noexcept {
  return a.f<b.f || (a.f==b.f &&
    (a.g<b.g || (a.g==b.g && a.node<b.node)));
}
// YIN: actual A* over a tree filtered by configuration and port state.
// On a violated guard, route approval halts rather than silently rerouting.
bool ruspt_astar(const Plan& p,int (&path)[nodes],
                 int& depth,int& total) noexcept {
  if(!policy_compliant(p) || !operational(p)) return false;
  Candidate queue[16]{};
  int best[nodes]={infinity,infinity,infinity,infinity,infinity};
  int length=1;
  best[root]=0;
  queue[0].node=root;queue[0].f=h[root];queue[0].g=0;
  queue[0].depth=1;queue[0].path[0]=root;
  while(length>0) {
    int index=0;
    for(int i=1;i<length;++i)
      if(earlier(queue[i],queue[index])) index=i;
    const Candidate current=queue[index];
    queue[index]=queue[--length];
    if(current.g!=best[current.node]) continue;
    if(current.node==goal) {
      depth=current.depth;total=current.g;
      for(int i=0;i<depth;++i) path[i]=current.path[i];
      return true;
    }
    if(current.depth>=nodes) continue;
    for(const Edge& e:graph) {
      if(e.source!=current.node ||
         !forwarding_edge(p,e.source,e.target) ||
         current.g>infinity-e.weight) continue;
      const int next=current.g+e.weight;
      if(next>=best[e.target]) continue;
      if(length>=16) return false;
      best[e.target]=next;
      Candidate c{};
      c.node=e.target;c.g=next;c.f=next+h[e.target];
      c.depth=current.depth+1;
      for(int i=0;i<current.depth;++i)
        c.path[i]=current.path[i];
      c.path[current.depth]=e.target;
      queue[length++]=c;
    }
  }
  return false;
}
struct Proof {
  bool yang_verified,yin_verified,coreao_verified;
  int path[nodes],depth,total,orientation_octant;
};
// DOVIA*: independent SPF(YANG) and protected A*(YIN), joined by COREAO.
bool dovia_star(const Plan& plan,Proof& proof) noexcept {
  proof=Proof{};
  proof.yang_verified=tree_proof(yang) && yang.cost[goal]==6;
  if(!proof.yang_verified ||
     !ruspt_astar(plan,proof.path,proof.depth,proof.total)) return false;
  proof.yin_verified=proof.depth==3 && proof.total==yang.cost[goal] &&
    proof.path[0]==root && proof.path[1]==2 && proof.path[2]==goal;
  proof.orientation_octant=turn45(turn45(0,3),1);
  proof.coreao_verified=proof.yin_verified &&
    proof.orientation_octant==4 && policy_compliant(plan) &&
    operational(plan);
  return proof.yang_verified && proof.yin_verified &&
    proof.coreao_verified;
}
} // namespace dovia
