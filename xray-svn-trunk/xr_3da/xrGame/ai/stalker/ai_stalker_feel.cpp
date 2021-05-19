
//	Module 		: ai_stalker_feel.cpp
//	Created 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Feelings for monster "Stalker"

#include "stdafx.h"
#include "ai_stalker.h"
#include "../../inventory_item.h"
#include "../../memory_manager.h"
#include "../../visual_memory_manager.h"
#include "../../sight_manager.h"
#include "../../stalker_movement_manager_smart_cover.h"
#include "../../stalker_animation_manager.h"

#ifdef DEBUG
#	include "../../ai_debug.h"
	extern Flags32 psAI_Flags;
#endif // DEBUG

BOOL CAI_Stalker::feel_vision_isRelevant(CObject* O)
{
	if (!g_Alive())
		return FALSE;

	CEntityAlive*	E = smart_cast<CEntityAlive*>(O);
	CInventoryItem*	I = smart_cast<CInventoryItem*>(O);

	if (!E && !I)
		return(FALSE);

	if (I && O->Position().distance_to(eye_matrix.c) > visionIgnoreItemsDist_) // Very important for perfomance of mt ai vision. Don't ray test all items in npc view frustum
		return FALSE;

	return(TRUE);
}

void CAI_Stalker::renderable_Render	(IRenderBuffer& render_buffer)
{
	inherited::renderable_Render(render_buffer);

#pragma note("Should it be like that?")
	if (!already_dead())
		CInventoryOwner::renderable_Render(render_buffer);

#ifdef DEBUG
	if (g_Alive()) {
		if (psAI_Flags.test(aiAnimationStats))
			animation().add_animation_stats	();
	}
#endif
}

void CAI_Stalker::Exec_Look(float dt)
{

	sight().Exec_Look(dt);
}

bool CAI_Stalker::bfCheckForNodeVisibility(u32 dwNodeID, bool bIfRayPick)
{
	return(memory().VisualMManager().visible(dwNodeID, movement().m_head.current.yaw, GetEyeFovValue()));
}

BOOL CAI_Stalker::feel_touch_contact(CObject* O)
{
	if (!m_take_items_enabled && smart_cast<CInventoryItem*>(O))
		return						(FALSE);

	if (O == this)
		return						(FALSE);

	if (!inherited::feel_touch_contact(O))
		return(FALSE);

	CGameObject* game_object = smart_cast<CGameObject*>(O);

	if (!game_object)
		return(FALSE);

	return(game_object->feel_touch_on_contact(this));
}

BOOL CAI_Stalker::feel_touch_on_contact	(CObject* O)
{
	VERIFY(O != this);

	if ((O->spatial.s_type | STYPE_VISIBLEFORAI) != O->spatial.s_type)
		return(FALSE);

	return(inherited::feel_touch_on_contact(O));
}

void CAI_Stalker::feel_touch_delete		(CObject* O)
{
	ignored_touched_objects_type::iterator	i = std::find(m_ignored_touched_objects.begin(), m_ignored_touched_objects.end(), O);
	if ( i == m_ignored_touched_objects.end() )
		return;

	m_ignored_touched_objects.erase			( i );
}