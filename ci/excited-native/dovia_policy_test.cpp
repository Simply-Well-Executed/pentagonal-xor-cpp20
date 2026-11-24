// SPDX-License-Identifier: 0BSD
// DOVIA-STAR-001: runtime policy, event and fail-closed negative tests.
#include "dovia_star.hpp"
int main() {
  if(!dovia::valid_single_view(dovia::single_view)) return 14;
  if(!dovia::inside_single_view(dovia::single_view,dovia::ViewSide::front,1500) ||
     dovia::inside_single_view(dovia::single_view,dovia::ViewSide::front,1501) ||
     !dovia::inside_single_view(dovia::single_view,dovia::ViewSide::rear,-600) ||
     dovia::inside_single_view(dovia::single_view,dovia::ViewSide::rear,-601))
    return 15;
  if(dovia::inside_single_view(dovia::SingleView{2u,3000,1200},
                               dovia::ViewSide::front,0) ||
     dovia::inside_single_view(dovia::SingleView{1u,3100,1200},
                               dovia::ViewSide::rear,0))
    return 16;
  dovia::Proof proof{};
  if(!dovia::dovia_star(dovia::baseline,proof)) return 1;
  if(!proof.yang_verified || !proof.yin_verified ||
     !proof.coreao_verified || proof.orientation_octant!=4 ||
     proof.total!=6 || proof.depth!=3 ||
     proof.path[0]!=0 || proof.path[1]!=2 || proof.path[2]!=4)
    return 2;
  if(dovia::dovia_star(dovia::bad_vlan(),proof)) return 3;
  if(dovia::dovia_star(dovia::stolen_root(),proof)) return 4;
  if(dovia::dovia_star(dovia::unguarded_downstream(),proof)) return 5;
  if(dovia::dovia_star(dovia::bad_channel(),proof)) return 6;
  if(dovia::dovia_star(dovia::edge_bpdu(),proof)) return 7;
  if(dovia::dovia_star(dovia::superior_bpdu(),proof)) return 8;
  dovia::Plan not_edge=dovia::baseline;
  not_edge.ports[8].portfast=false;
  if(dovia::dovia_star(not_edge,proof)) return 9;
  dovia::Plan wrong_root_port=dovia::baseline;
  wrong_root_port.ports[1].root_guard=true;
  if(dovia::dovia_star(wrong_root_port,proof)) return 10;
  dovia::Plan channel_mismatch=dovia::baseline;
  channel_mismatch.members[1].vlan_first=2;
  if(dovia::dovia_star(channel_mismatch,proof)) return 11;
  dovia::Plan bridge_id_collision=dovia::baseline;
  bridge_id_collision.bridges[2].stable_id=1ull;
  if(dovia::dovia_star(bridge_id_collision,proof)) return 12;
  if(!dovia::dovia_star(dovia::baseline,proof)) return 13;
  return 0;
}
