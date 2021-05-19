#ifndef IGame_PersistentH
#define IGame_PersistentH
#pragma once

#include "Environment.h"
#include "ShadersExternalData.h" 
#include "IGame_ObjectPool.h"

class IRenderVisual;
class IMainMenu;

class ENGINE_API IGame_Persistent : 
#ifndef _EDITOR
	public DLL_Pure,
#endif

	public pureAppStart, 
	public pureAppEnd,
	public pureAppActivate, 
	public pureAppDeactivate,
	public pureFrame,
	public pureRender
{
public:
	union params {
		struct {
			string256	m_game_or_spawn;
			string256	m_game_type;
			string256	m_alife;
			string256	m_new_or_load;
		};

		string256		m_params[4];

						params		()	{ reset(); }

		void			reset		()
		{
			for (int i = 0; i < 4; ++i)
				xr_strcpy (m_params[i], "");
		}

		void						parse_cmd_line(LPCSTR cmd_line)
		{
			reset					();

			int						n = _min(4,_GetItemCount(cmd_line,'/'));

			for (int i = 0; i < n; ++i)
			{
				_GetItem			(cmd_line, i, m_params[i], '/');
				strlwr				(m_params[i]);
			}
		}
	};
	params							m_game_params;

	virtual void					PreStart			(LPCSTR op);
	virtual void					Start				(LPCSTR op);
	virtual void					Disconnect			();

#ifndef _EDITOR
	IGame_ObjectPool				ObjectPool;
	CEnvironment*					levelEnvironment;

	CEnvironment&					Environment			() { return *levelEnvironment; };
	CEnvironment*					EnvironmentP		() { return levelEnvironment; };

	xr_vector<shared_str>			SuggestedForPrefetchingUI;

	void							ReportUITxrsForPrefetching(); //Prints the list of UI textures, which caused stutterings during game
#endif
	IMainMenu*						m_pMainMenu;

	WeaponSVPParamsShExport*		weaponSVPShaderParams; //--#SM+#--

	// Runs scripted weather cycle selection
	virtual void					SelectWeatherCycle() {};
	// Checks if weather script has been loaded to prevent undefined behavior
	virtual bool					IsWeatherScriptLoaded() { return false; };

	virtual bool					OnRenderPPUI_query	() { return FALSE; };	// should return true if we want to have second function called
	virtual void					OnRenderPPUI_main	() {};
	virtual void					OnRenderPPUI_PP		() {};

	virtual	void					OnAppStart			();
	virtual void					OnAppEnd			();
	virtual	void					OnAppActivate		();
	virtual void					OnAppDeactivate		();
	virtual void					OnFrame				();
	virtual void					OnRender			();

	// вызывается только когда изменяется тип игры
	virtual	void					OnGameStart			(); 
	virtual void					OnGameEnd			();

	bool __stdcall					Prefetching_stage();

	virtual void					GetCurrentDof		(Fvector3& dof){dof.set(-1.4f, 0.0f, 250.f);};
	virtual void					SetBaseDof			(const Fvector3& dof){};
	virtual void					OnSectorChanged		(int sector){};
	virtual void					OnAssetsChanged		();

	virtual void					RegisterModel(IRenderVisual* V)

#ifndef _EDITOR
     = 0;
#else
	{}
#endif
	virtual	float					MtlTransparent		(u32 mtl_idx)
#ifndef _EDITOR
	= 0;
#else
	{return 1.f;}
#endif

	IGame_Persistent				();
	virtual ~IGame_Persistent		();

	virtual	void					LoadTitle			(LPCSTR ui_msg, LPCSTR log_msg, bool change_tip = false, shared_str map_name = ""){}
	virtual bool					CanBePaused			()		{ return true;}

	bool							render_scene;
	IC bool							SceneRenderingBlocked();
};

class IMainMenu
{
public:
	virtual			~IMainMenu						()													{};
	virtual void	Activate						(bool bActive)										=0; 
	virtual	bool	IsActive						()													=0; 
	virtual	bool	CanSkipSceneRendering			()													=0; 
	virtual void	DestroyInternal					(bool bForce)										=0;
};

extern ENGINE_API	IGame_Persistent*	g_pGamePersistent;

#endif
