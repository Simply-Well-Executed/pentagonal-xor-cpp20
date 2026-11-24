// SPDX-License-Identifier: 0BSD
// EXCITED-STAR-002: raw C++20 A*, no headers or C++ standard library.
// Win32 native stdout replaces the sample's Linux POSIX file descriptors.
extern "C" __declspec(dllimport) void* __stdcall GetStdHandle(unsigned long);
extern "C" __declspec(dllimport) int __stdcall WriteFile(
  void*,const void*,unsigned long,unsigned long*,void*);

namespace excited {
constexpr int count=5,start=0,goal=4,inf=999999;
constexpr const char* names[count]={"Overview","Architecture","Security","Benchmarks","Summary"};
constexpr int cost[count][count]={
  {inf,2,4,inf,inf},
  {inf,inf,inf,3,5},
  {inf,inf,inf,inf,2},
  {inf,inf,inf,inf,1},
  {inf,inf,inf,inf,inf}
};
constexpr int heuristic[count]={4,3,2,1,0};
constexpr bool consistent() noexcept {
  if(heuristic[goal]!=0) return false;
  for(int i=0;i<count;++i) for(int j=0;j<count;++j)
    if(cost[i][j]<inf && (cost[i][j]<0 ||
        heuristic[i]>cost[i][j]+heuristic[j])) return false;
  return true;
}
static_assert(consistent());
static_assert(cost[0][1]+cost[1][3]+cost[3][4]==6);
static_assert(cost[0][2]+cost[2][4]==6);
struct Entry{int f,g,node,depth,path[count];};
constexpr bool better(const Entry& a,const Entry& b) noexcept {
  return a.f!=b.f ? a.f<b.f : a.g!=b.g ? a.g<b.g : a.node<b.node;
}
bool astar(int (&route)[count],int& depth,int& total) noexcept {
  Entry q[64]{};
  int best[count]={inf,inf,inf,inf,inf};
  int len=1;
  best[start]=0;
  q[0].f=heuristic[start];q[0].g=0;q[0].node=start;
  q[0].depth=1;q[0].path[0]=start;
  while(len>0) {
    int idx=0;
    for(int i=1;i<len;++i) if(better(q[i],q[idx])) idx=i;
    const Entry cur=q[idx];
    q[idx]=q[--len];
    if(cur.g!=best[cur.node]) continue;
    if(cur.node==goal) {
      depth=cur.depth;total=cur.g;
      for(int i=0;i<depth;++i) route[i]=cur.path[i];
      return true;
    }
    if(cur.depth>=count) continue;
    for(int next=0;next<count;++next) {
      const int w=cost[cur.node][next];
      if(w>=inf || cur.g>inf-w) continue;
      const int g=cur.g+w;
      if(g>=best[next]) continue;
      if(len>=64) return false;
      best[next]=g;
      Entry value{};
      value.g=g;value.f=g+heuristic[next];value.node=next;
      value.depth=cur.depth+1;
      for(int i=0;i<cur.depth;++i) value.path[i]=cur.path[i];
      value.path[cur.depth]=next;
      q[len++]=value;
    }
  }
  return false;
}
// SPT Finland: a shortest-path-tree witness, NOT the Ethernet STP protocol.
// A deterministic Dijkstra pass, independently checking the A* goal cost.
struct SpTree {
  int distance[count];
  int parent[count];
  bool settled[count];
};
constexpr SpTree shortest_path_tree() noexcept {
  SpTree result{};
  for(int i=0;i<count;++i) {
    result.distance[i]=inf;
    result.parent[i]=-1;
    result.settled[i]=false;
  }
  result.distance[start]=0;
  for(int iteration=0;iteration<count;++iteration) {
    int u=-1;
    for(int n=0;n<count;++n) {
      if(!result.settled[n] &&
         (u<0 || result.distance[n]<result.distance[u] ||
          (result.distance[n]==result.distance[u] && n<u))) {
        u=n;
      }
    }
    if(u<0 || result.distance[u]==inf) break;
    result.settled[u]=true;
    for(int v=0;v<count;++v) {
      const int edge=cost[u][v];
      if(result.settled[v] || edge==inf) continue;
      if(edge<0 || result.distance[u]>inf-edge) continue;
      const int candidate=result.distance[u]+edge;
      if(candidate<result.distance[v]) {
        result.distance[v]=candidate;
        result.parent[v]=u;
      }
    }
  }
  return result;
}
constexpr bool valid_spt(const SpTree& tree) noexcept {
  if(tree.distance[start]!=0 || tree.parent[start]!=-1) return false;
  for(int v=0;v<count;++v) {
    if(!tree.settled[v] || tree.distance[v]>=inf) return false;
    if(v!=start) {
      const int p=tree.parent[v];
      if(p<0 || p>=count || cost[p][v]==inf ||
         tree.distance[p]>inf-cost[p][v] ||
         tree.distance[p]+cost[p][v]!=tree.distance[v]) return false;
      // Every parent chain reaches the one root, so the tree has no cycles.
      int current=v;
      int hops=0;
      while(current!=start && hops<count) {
        current=tree.parent[current];
        if(current<0 || current>=count) return false;
        ++hops;
      }
      if(current!=start) return false;
    }
    // Bellman optimality against every traversable edge.
    for(int next=0;next<count;++next) {
      const int w=cost[v][next];
      if(w!=inf && (w<0 || tree.distance[v]>inf-w ||
                     tree.distance[next]>tree.distance[v]+w)) return false;
    }
  }
  return true;
}
inline constexpr SpTree spt_finland=shortest_path_tree();
static_assert(valid_spt(spt_finland));
static_assert(spt_finland.distance[0]==0 && spt_finland.distance[1]==2 &&
              spt_finland.distance[2]==4 && spt_finland.distance[3]==5 &&
              spt_finland.distance[4]==6);
static_assert(spt_finland.parent[0]==-1 && spt_finland.parent[1]==0 &&
              spt_finland.parent[2]==0 && spt_finland.parent[3]==1 &&
              spt_finland.parent[4]==2);
bool same_optimum_as_spt(const int (&route)[count],int depth,int total) noexcept {
  if(!valid_spt(spt_finland) || depth<2 || depth>count ||
     total!=spt_finland.distance[goal] || route[0]!=start ||
     route[depth-1]!=goal) return false;
  int sum=0;
  bool seen[count]{};
  for(int i=0;i<depth;++i) {
    const int node=route[i];
    if(node<0 || node>=count || seen[node]) return false;
    seen[node]=true;
    if(i==0)continue;
    const int prior=route[i-1];
    const int weight=cost[prior][node];
    if(weight==inf || weight<0 || sum>inf-weight) return false;
    sum+=weight;
  }
  return sum==total;
}
struct Text {
  char data[4096]{};
  unsigned long size=0;
  bool append(const char* value) noexcept {
    for(unsigned long i=0;value[i]!=0;++i) {
      if(size>=sizeof(data)-1u) return false;
      data[size++]=value[i];
    }
    data[size]=0;
    return true;
  }
  bool numeral(unsigned int v) noexcept {
    char digits[16]{};
    unsigned int n=0;
    do {
      digits[n++]=static_cast<char>('0'+v%10u);
      v/=10u;
    }while(v>0u && n<16u);
    if(v>0u) return false;
    while(n>0u) {
      if(size>=sizeof(data)-1u) return false;
      data[size++]=digits[--n];
    }
    data[size]=0;
    return true;
  }
};
bool markdown(const int (&path)[count],int depth,int total,Text& out) noexcept {
  if(depth<2 || depth>count || total<0 || path[0]!=start ||
     path[depth-1]!=goal) return false;
  if(!out.append("# Sovieta Diao Xian ^.^ | EXCITED v2 A* Markdown\n\n") ||
     !out.append("**Optimal path:** ")) return false;
  for(int i=0;i<depth;++i) {
    if(path[i]<0 || path[i]>=count) return false;
    if(i!=0 && !out.append(" -> ")) return false;
    if(!out.append(names[path[i]])) return false;
  }
  if(!out.append("\n\n**Total path cost:** ") ||
     !out.numeral(static_cast<unsigned int>(total)) ||
     !out.append("\n\n## Selected sections\n\n")) return false;
  for(int i=0;i<depth;++i) {
    if(!out.numeral(static_cast<unsigned int>(i+1)) ||
       !out.append(". **") || !out.append(names[path[i]]) ||
       !out.append("**: selected by deterministic A*.\n")) return false;
  }
  if(!out.append("\n## SPT Finland shortest-path tree cross-check\n\n") ||
     !out.append("SPT = Shortest Path Tree (Dijkstra verification); ")
     || !out.append("it is not the Ethernet Spanning Tree Protocol (STP).\n\n")
     || !out.append("**SPT root:** Overview\n")
     || !out.append("**SPT total goal cost:** ")
     || !out.numeral(static_cast<unsigned int>(spt_finland.distance[goal]))
     || !out.append("\n\n### SPT vertex distances\n\n")) return false;
  for(int i=0;i<count;++i) {
    if(!out.append("- ") || !out.append(names[i]) ||
       !out.append(": cost ") ||
       !out.numeral(static_cast<unsigned int>(spt_finland.distance[i])) ||
       !out.append(", parent ") ||
       !(spt_finland.parent[i]<0 ? out.append("ROOT") :
         out.append(names[spt_finland.parent[i]])) ||
       !out.append("\n")) return false;
  }
  return out.append(
    "\n## COREAO code-block completion\n\n"
    "EXCITE qualifies the file and subproject circuits before this builder "
    "runs. QCHART -> QLAM -> QGRAPH -> inverse-square rational witnesses "
    "-> FRESH HAODA -> QCODE is a user-proposed evidence chain, not a "
    "physical radiation measurement.\n\n"
    "The inverse-square examples are exact dimensionless fractions "
    "1/1, 1/4 and 1/9. Canonical source identity is unchanged. "
    "Failed laps remain HOLD and historical receipts are preserved.\n\n"
    "## Authorization and audit\n\n"
    "This generator never rewrites a source file, acknowledges INCEPTION, "
    "closes an issue, merges a pull request or authorizes publication. "
    "Independent verification and human approval remain required.\n\n"
    "**RELEASE=HOLD**\n");
}
bool emit(const Text& data) noexcept {
  void* handle=GetStdHandle(static_cast<unsigned long>(-11));
  if(handle==nullptr || handle==reinterpret_cast<void*>(-1)) return false;
  unsigned long pos=0;
  while(pos<data.size) {
    unsigned long written=0;
    if(WriteFile(handle,data.data+pos,data.size-pos,&written,nullptr)==0 ||
       written==0u) return false;
    pos+=written;
  }
  return true;
}
}
int main() {
  int route[excited::count]{};
  int depth=0,total=0;
  if(!excited::astar(route,depth,total) || total!=6 || depth!=3 ||
     route[0]!=0 || route[1]!=2 || route[2]!=4 ||
     !excited::same_optimum_as_spt(route,depth,total)) return 1;
  excited::Text output{};
  if(!excited::markdown(route,depth,total,output)) return 2;
  return excited::emit(output) ? 0 : 3;
}
