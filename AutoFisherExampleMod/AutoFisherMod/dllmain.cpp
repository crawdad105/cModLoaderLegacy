#include "pch.h"

// set platform to x86 
// refrence paths (change them based off of your drive locations)
// C:\Program Files (x86)\Steam\steamapps\common\tModLoader\Libraries\ReLogic\1.0.0\ReLogic.dll
// C:\Program Files (x86)\Steam\steamapps\common\Terraria\Terraria.exe (if cModLoader is installed use C:\Program Files (x86)\Steam\steamapps\common\Terraria\RealTerraria.exe)
// C:\Windows\Microsoft.NET\assembly\GAC_32\Microsoft.Xna.Framework\v4.0_4.0.0.0__842cf8be1de50553\Microsoft.Xna.Framework.dll
// C:\Windows\Microsoft.NET\assembly\GAC_32\Microsoft.Xna.Framework.Game\v4.0_4.0.0.0__842cf8be1de50553\Microsoft.Xna.Framework.Game.dll
// C:\Windows\Microsoft.NET\assembly\GAC_32\Microsoft.Xna.Framework.Graphics\v4.0_4.0.0.0__842cf8be1de50553\Microsoft.Xna.Framework.Graphics.dll
// (you need to add the cModLoader dll aswell)

ref class ConfigOption : Terraria::GameContent::UI::Elements::UIPanel {

	void OnTextHover(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement) {
		Terraria::GameContent::UI::Elements::UIText^ text = (Terraria::GameContent::UI::Elements::UIText^)listeningElement;
		Terraria::Audio::SoundEngine::PlaySound(12, -1, -1, 1, 1, 0);
		text->TextColor = Terraria::ID::Colors::FancyUIFatButtonMouseOver;
	}
	void OnTextLeave(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement) {
		Terraria::GameContent::UI::Elements::UIText^ text = (Terraria::GameContent::UI::Elements::UIText^)listeningElement;
		text->TextColor = Microsoft::Xna::Framework::Color::Color(255, 255, 255);
	}
	void OnTextClick(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement) {
		Terraria::GameContent::UI::Elements::UIText^ text = (Terraria::GameContent::UI::Elements::UIText^)listeningElement;
		bool _old = false;
		bool _new = true;
		if (text->Text == "[On]") {
			_old = true;
			_new = false;
			text->SetText("[Off]");
		}
		else {
			text->SetText("[On]");
		}
		OnOptionValueChanged(_old, _new);
		Value = _new;
	}

public:
	enum class OptionType {
		Boolean,
		Number, // not used
		Float,  // not used
		String, // not used
		None
	};
	ConfigOption(System::String^ str, OptionType type, System::Object^ defaultValue) {
		Width.Set(0, 1);
		Height.Set(42, 0);
		SetPadding(6);
		auto text = gcnew Terraria::GameContent::UI::Elements::UIText(str, 1, false);
		text->Left.Set(6, 0);
		text->Top.Set(4, 0);
		Append(text);

		Value = defaultValue;

		Terraria::GameContent::UI::Elements::UIText^ checkText = nullptr;

		switch (type)
		{
		case ConfigOption::OptionType::Boolean:
			checkText = gcnew Terraria::GameContent::UI::Elements::UIText("[" + ((bool)defaultValue == true ? "On" : "Off") + "]", 1, false);
			checkText->Left.Set(-6, 0);
			checkText->Top.Set(4, 0);
			checkText->HAlign = 1;

			checkText->OnMouseOver += gcnew Terraria::UI::UIElement::MouseEvent(this, &ConfigOption::OnTextHover);
			checkText->OnMouseOut += gcnew Terraria::UI::UIElement::MouseEvent(this, &ConfigOption::OnTextLeave);
			checkText->OnLeftClick += gcnew Terraria::UI::UIElement::MouseEvent(this, &ConfigOption::OnTextClick);

			Append(checkText);
			break;
		case ConfigOption::OptionType::Number:
			break;
			// scraped idea
		case ConfigOption::OptionType::Float:
			break;
			// scraped idea
		case ConfigOption::OptionType::String:
			break;
			// scraped idea
		default:
			break;
		}

	}

	System::Object^ Value = nullptr;

	void OptionUpdate() {
		OnOptionUpdate(Value);
	}

	delegate void OptionEventUpdate(System::Object^ value);
	delegate void OptionEventChange(System::Object^ oldValue, System::Object^ newValue);
	event OptionEventUpdate^ OnOptionUpdate;
	event OptionEventChange^ OnOptionValueChanged;

};

// class initialization mimics Terraria.GameContent.UI.States.UIAchievementsMenu
ref class CustomConfigMenu : Terraria::UI::UIState {
	Terraria::GameContent::UI::Elements::UIList^ _List;
	Terraria::GameContent::UI::Elements::UIScrollbar^ _ScrollBar;
	bool _isScrollbarAttached = false;
	System::Collections::Generic::List<Terraria::GameContent::UI::Elements::UIText^>^ _listElements = gcnew System::Collections::Generic::List<Terraria::GameContent::UI::Elements::UIText^>(0);
	Terraria::UI::UIElement^ _backpanel;
	Terraria::UI::UIElement^ _outerContainer;

	static void GoBackClick(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement) {
		Terraria::Main::menuMode = 0;
		Terraria::UI::IngameFancyUI::Close();
	}
	static void FadedMouseOver(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement) {
		Terraria::Audio::SoundEngine::PlaySound(12, -1, -1, 1, 1, 0);
		((Terraria::GameContent::UI::Elements::UIPanel^)evt->Target)->BackgroundColor = Microsoft::Xna::Framework::Color::Color(73, 94, 171);
		((Terraria::GameContent::UI::Elements::UIPanel^)evt->Target)->BorderColor = Terraria::ID::Colors::FancyUIFatButtonMouseOver;
	}
	static void FadedMouseOut(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement) {
		((Terraria::GameContent::UI::Elements::UIPanel^)evt->Target)->BackgroundColor = Microsoft::Xna::Framework::Color::Color(63, 82, 151) * 0.8f;
		((Terraria::GameContent::UI::Elements::UIPanel^)evt->Target)->BorderColor = Microsoft::Xna::Framework::Color::Black;
	}

	void defaultUpdateFunc(System::Object^ value) { }
	void defaultChangeFunc(System::Object^ oldValue, System::Object^ newValue) { }

	void Option_ChangeCatchFish(System::Object^ oldValue, System::Object^ newValue) {
		Option_CatchFish = (bool)newValue;
	}
	void Option_ChangeInstantCatch(System::Object^ oldValue, System::Object^ newValue) {
		Option_InstantCatch = (bool)newValue;
	}
	void Option_ChangeOnlyCrates(System::Object^ oldValue, System::Object^ newValue) {
		Option_OnlyCrates = (bool)newValue;
	}
	void Option_ChangeDrinkPotions(System::Object^ oldValue, System::Object^ newValue) {
		Option_DrinkPotions = (bool)newValue;
	}
	void Option_ChangePauseHurt(System::Object^ oldValue, System::Object^ newValue) {
		Option_PauseHurt = (bool)newValue;
	}

	void AddDefualtOptions() {
		AddBoolOption("Auto fisher", true, gcnew ConfigOption::OptionEventChange(this, &CustomConfigMenu::Option_ChangeCatchFish));
		AddBoolOption("Instant Catch [Cheat]", false, gcnew ConfigOption::OptionEventChange(this, &CustomConfigMenu::Option_ChangeInstantCatch));	
		AddBoolOption("Catch Crates Only", false, gcnew ConfigOption::OptionEventChange(this, &CustomConfigMenu::Option_ChangeOnlyCrates));
		AddBoolOption("Drink Potions (fishing and crate)", false, gcnew ConfigOption::OptionEventChange(this, &CustomConfigMenu::Option_ChangeDrinkPotions));
		AddBoolOption("Pause When Hurt (turns off when hurt)", false, gcnew ConfigOption::OptionEventChange(this, &CustomConfigMenu::Option_ChangePauseHurt));
	}

public: 

	static bool Option_CatchFish = true;
	static bool Option_InstantCatch = false;
	static bool Option_OnlyCrates = false;
	static bool Option_DrinkPotions = false;
	static bool Option_PauseHurt = false;

	void OnInitialize() override {
		
		_outerContainer = gcnew Terraria::UI::UIElement();
		_outerContainer->Width.Set(0, 0.6f);
		_outerContainer->MaxWidth.Set(600, 0);
		_outerContainer->MinWidth.Set(400, 0);
		_outerContainer->Top.Set(100, 0);
		_outerContainer->Height.Set(-100, 1);
		_outerContainer->HAlign = 0.5f;
		Append(_outerContainer);

		Terraria::GameContent::UI::Elements::UIPanel^ uIPanel = gcnew Terraria::GameContent::UI::Elements::UIPanel();
		uIPanel->Width.Set(0, 1);
		uIPanel->Height.Set(-110, 1);
		uIPanel->BackgroundColor = Microsoft::Xna::Framework::Color::Color(33, 43, 79) * 0.8f;
		uIPanel->PaddingTop = 0;
		_outerContainer->Append(uIPanel);

		_List = gcnew Terraria::GameContent::UI::Elements::UIList();
		_List->Width.Set(-25, 1);
		_List->Height.Set(-50, 1);
		_List->Top.Set(50, 0);
		_List->ListPadding = 5;
		uIPanel->Append(_List);

		Terraria::GameContent::UI::Elements::UITextPanel<System::String^>^ uITextPanel = 
			gcnew Terraria::GameContent::UI::Elements::UITextPanel<System::String^>("Auto Fisher Config", 1, true);
		uITextPanel->HAlign = 0.5f;
		uITextPanel->Top.Set(-33, 0);
		uITextPanel->SetPadding(13);
		uITextPanel->BackgroundColor = Microsoft::Xna::Framework::Color::Color(73, 94, 171);
		_outerContainer->Append(uITextPanel);

		Terraria::GameContent::UI::Elements::UITextPanel<System::String^>^ uITextPanel2 =
			gcnew Terraria::GameContent::UI::Elements::UITextPanel<System::String^>("Back", 0.7f, true);
		uITextPanel2->Width.Set(-10, 0.5f);
		uITextPanel2->Height.Set(50, 0);
		uITextPanel2->VAlign = 1;
		uITextPanel2->HAlign = 0.5f;
		uITextPanel2->Top.Set(-45, 0);
		uITextPanel2->OnMouseOver += gcnew UIElement::MouseEvent(&FadedMouseOver);
		uITextPanel2->OnMouseOut += gcnew UIElement::MouseEvent(&FadedMouseOut);
		uITextPanel2->OnLeftClick += gcnew UIElement::MouseEvent(&GoBackClick);
		_outerContainer->Append(uITextPanel2);

		_ScrollBar = gcnew Terraria::GameContent::UI::Elements::UIScrollbar();
		_ScrollBar->SetView(100, 1000);
		_ScrollBar->Height.Set(-50, 1);
		_ScrollBar->Top.Set(50, 0);
		_ScrollBar->HAlign = 1;
		uIPanel->Append(_ScrollBar);
		_List->SetScrollbar(_ScrollBar);

		AddDefualtOptions();

	}

	void AddBoolOption(System::String^ optionText, System::Object^ value, ConfigOption::OptionEventChange^ changeFunc) {
		AddOption(optionText, ConfigOption::OptionType::Boolean, value,
			gcnew ConfigOption::OptionEventUpdate(this, &CustomConfigMenu::defaultUpdateFunc),
			changeFunc
		);
	}
	void AddOption(System::String^ optionText, ConfigOption::OptionType type, System::Object^ value, ConfigOption::OptionEventUpdate^ updateFunc, ConfigOption::OptionEventChange^ changeFunc) {

		try
		{
			auto option = gcnew ConfigOption(optionText, type, value);
			option->OnOptionUpdate += updateFunc;
			option->OnOptionValueChanged += changeFunc;
			_List->Add(option);
		}
		catch (System::Exception^ e)
		{
			auto option = gcnew ConfigOption("ERROR: " + e->Message, ConfigOption::OptionType::None, nullptr);
			option->OnOptionUpdate += updateFunc;
			option->OnOptionValueChanged += changeFunc;
			_List->Add(option);
		}

		if (_isScrollbarAttached && !_ScrollBar->CanScroll) {
			_outerContainer->RemoveChild(_ScrollBar);
			_isScrollbarAttached = false;
			_List->Width.Set(0, 1);
		}
		else if (!_isScrollbarAttached && _ScrollBar->CanScroll) {
			_outerContainer->Append(_ScrollBar);
			_isScrollbarAttached = true;
			_List->Width.Set(-25, 1);
		}
	}

	void UpdateOptions() {
		auto _itemsField = (Terraria::GameContent::UI::Elements::UIList::typeid)->GetField("_items", System::Reflection::BindingFlags::NonPublic | System::Reflection::BindingFlags::Instance);
		auto list = (System::Collections::Generic::List<Terraria::UI::UIElement^>^)(_itemsField->GetValue(_List));
		for (int i = 0; i < list->Count; i++) {
			Terraria::UI::UIElement^ elm = list[i];
			if (elm->GetType() == ConfigOption::typeid) {
				((ConfigOption^)elm)->OptionUpdate();
			}
		}
	}

};

ref class CustomUI : Terraria::UI::UIState {
	void OnClickCompact(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement) {
		compactMode = !compactMode;
		isDraging = false;
	}
	void OnClickConfig(Terraria::UI::UIMouseEvent^ evt, Terraria::UI::UIElement^ listeningElement) {

		// the fallowing code was inspired by Terraria.UI.IngameFancyUI.OpenAchievements()

		Terraria::Main::playerInventory = false;
		Terraria::Main::editChest = false;
		Terraria::Main::npcChatText = "";
		Terraria::Main::inFancyUI = true;
		
		Terraria::Main::InGameUI->SetState(customConfigMenu);
		isDraging = false;

	}
public:

	static array<int>^ fishingPoles = {
		Terraria::ID::ItemID::WoodFishingPole,
		Terraria::ID::ItemID::ReinforcedFishingPole,
		Terraria::ID::ItemID::FisherofSouls,
		Terraria::ID::ItemID::Fleshcatcher,
		Terraria::ID::ItemID::ScarabFishingRod,
		Terraria::ID::ItemID::BloodFishingRod,
		Terraria::ID::ItemID::FiberglassFishingPole,
		Terraria::ID::ItemID::MechanicsRod,
		Terraria::ID::ItemID::SittingDucksFishingRod,
		Terraria::ID::ItemID::HotlineFishingHook,
		Terraria::ID::ItemID::GoldenFishingRod
	};
	static array<int>^ fishingBobbers = {
		Terraria::ID::ProjectileID::BobberWooden,
		Terraria::ID::ProjectileID::BobberReinforced,
		Terraria::ID::ProjectileID::BobberFisherOfSouls,
		Terraria::ID::ProjectileID::BobberFleshcatcher,
		Terraria::ID::ProjectileID::BobberScarab,
		Terraria::ID::ProjectileID::BobberBloody,
		Terraria::ID::ProjectileID::BobberFiberglass,
		Terraria::ID::ProjectileID::BobberMechanics,
		Terraria::ID::ProjectileID::BobbersittingDuck,
		Terraria::ID::ProjectileID::BobberHotline,
		Terraria::ID::ProjectileID::BobberGolden
	};

	Terraria::GameContent::UI::Elements::UIPanel^ Panel;
	Terraria::GameContent::UI::Elements::UIText^ Text;
	Terraria::GameContent::UI::Elements::UIImageButton^ minimizeButton;
	Terraria::GameContent::UI::Elements::UIImageButton^ configButton;
	ReLogic::Content::Asset<Microsoft::Xna::Framework::Graphics::Texture2D^>^ _moreImg;
	ReLogic::Content::Asset<Microsoft::Xna::Framework::Graphics::Texture2D^>^ _lessImg;
	ReLogic::Content::Asset<Microsoft::Xna::Framework::Graphics::Texture2D^>^ _configImg;

	CustomConfigMenu^ customConfigMenu;
	int PosX;
	int PosY;
	bool isDraging;
	Microsoft::Xna::Framework::Vector2 DragOffset;
	bool compactMode = false;
	bool coughtFish = false;
	bool shouldCastBobber = false;
	Microsoft::Xna::Framework::Vector2 lastBobberPos;
	void Inistialize() {
		customConfigMenu = gcnew CustomConfigMenu();
		customConfigMenu->Initialize(); // initalize now so CustomConfigMenu.UpdateOptions() works
		compactMode = false;
		isDraging = false;
		PosX = 0;
		PosY = 0;

		Panel = gcnew Terraria::GameContent::UI::Elements::UIPanel();
		Panel->MinWidth.Set(300, 0);
		Panel->MinHeight.Set(300, 0);

		Text = gcnew Terraria::GameContent::UI::Elements::UIText("Some Text", 1, false);

		System::String^ moreStr = "Images/UI/Bestiary/Button_Forward";
		System::String^ lessStr = "Images/UI/Bestiary/Button_Back";
		_moreImg = Terraria::Main::Assets->Request<Microsoft::Xna::Framework::Graphics::Texture2D^>(moreStr, ReLogic::Content::AssetRequestMode::ImmediateLoad);
		_lessImg = Terraria::Main::Assets->Request<Microsoft::Xna::Framework::Graphics::Texture2D^>(lessStr, ReLogic::Content::AssetRequestMode::ImmediateLoad);
		minimizeButton = gcnew Terraria::GameContent::UI::Elements::UIImageButton(_lessImg);
		minimizeButton->OnLeftMouseDown += gcnew Terraria::UI::UIElement::MouseEvent(this, &CustomUI::OnClickCompact);
		minimizeButton->Left.Set(-22, 1);
		minimizeButton->Top.Set(-6, 0);

		System::String^ configStr = "Images/UI/Creative/Research_GearB";
		_configImg = Terraria::Main::Assets->Request<Microsoft::Xna::Framework::Graphics::Texture2D^>(configStr, ReLogic::Content::AssetRequestMode::ImmediateLoad);
		configButton = gcnew Terraria::GameContent::UI::Elements::UIImageButton(_configImg);
		configButton->OnLeftMouseDown += gcnew Terraria::UI::UIElement::MouseEvent(this, &CustomUI::OnClickConfig);
		configButton->Left.Set(-22, 1);
		configButton->Top.Set(24, 0);

		Append(Panel);
		Panel->Append(Text);
		Panel->Append(minimizeButton);
		Panel->Append(configButton);

	};
	void DrawSelf(Microsoft::Xna::Framework::Graphics::SpriteBatch^ sb) override {

		auto mousePos = Terraria::Main::MouseScreen;
		
		if (Panel->ContainsPoint(mousePos)) {
			cModLoader::ModHelper::userPlayer->mouseInterface = true;
			if (cModLoader::ModHelper::wasLeftMousePressDown) {
				isDraging = true;
				DragOffset = Microsoft::Xna::Framework::Vector2(mousePos.X - PosX, mousePos.Y - PosY);
			}
		}
		if (cModLoader::ModHelper::wasLeftMousePressUp) {
			isDraging = false;
		}
		if (isDraging) {
			auto newPos = Microsoft::Xna::Framework::Vector2(mousePos.X - DragOffset.X, mousePos.Y - DragOffset.Y);
			PosX = newPos.X;
			PosY = newPos.Y;
		}

		int height = compactMode ? 80 : 300;
		int width = compactMode ? 250 : 400;

		if (PosX < 0) PosX = 0;
		if (PosY < 0) PosY = 0;
		if ((PosX + width) > Terraria::Main::screenWidth) PosX = (Terraria::Main::screenWidth - width);
		if ((PosY + height) > Terraria::Main::screenHeight) PosY = (Terraria::Main::screenHeight - height);

		//Panel->MinHeight.Set(height, 0);
		Panel->Left.Set(PosX, 0);
		Panel->Top.Set(PosY, 0);
		Panel->MinWidth.Set(width, 0);
		Panel->MinHeight.Set(height, 0);
		Panel->Recalculate();

		System::String^ displayText;

		try {
			displayText = UpdateFisher();
		}
		catch (System::Exception^ e) {
			displayText = e->StackTrace + "\n--------\n" + e->Message;
		}
		Text->SetText(displayText, 1, false);

		Terraria::UI::UIElement::DrawSelf(sb);
	
		minimizeButton->SetImage(compactMode ? _moreImg : _lessImg);

	}

	int GetFishingPole(Terraria::Player^ player) {
		for (int i = 0; i < fishingPoles->Length; i++) {
			if (fishingPoles[i] == player->HeldItem->type) {
				// return index of the fishing rod in `fishingPoles`
				return i;
				break;
			}
		}
		// return -1 if not holding a fishing rod
		return -1;
	}
	Terraria::Projectile^ GetBobber(Terraria::Player^ player) {
		int rodType = GetFishingPole(player);
		if (rodType == -1) return nullptr;
		for (int i = 0; i < Terraria::Main::projectile->Length; i++) {
			auto proj = Terraria::Main::projectile[i];
			if (proj->active && proj->owner == Terraria::Main::myPlayer && proj->type == fishingBobbers[rodType]) {
				return proj;
				break;
			}
		}
		return nullptr;
	}
	/// <summary>
	/// Gets player fishing information and ruins them through some code to check if the program should catch or throw the bobber
	/// </summary>
	/// <returns>String realting to the state of this function</returns>
	System::String^ UpdateFisher(){

		customConfigMenu->UpdateOptions();

		System::String^ returnString = "";

		// get player and player item id
		auto player = cModLoader::ModHelper::userPlayer;
		int itemId = player->HeldItem->type;

		// check if user is holdin fishing rod
		int rodType = GetFishingPole(player);
		// if the player is not holding a fishing rod return early 
		if (rodType == -1) {
			return "Hold a fishing rod.";
		}

		// check if bober exist
		Terraria::Projectile^ Bobber = GetBobber(player);
		if (Bobber == nullptr) {
			// if no bobbers were found and if coughtFish is true it means a fish was cought by this program
			//  so we throw the bobber
			if (coughtFish) {
				// set shouldCostBobber to true, so next time this fucntions (UpdateFisher) is ran it runs other code
				shouldCastBobber = true;
				DoCastBobber(player);
				return "Cast a bobber.";
			}
			return "No bobber found.";
		}
		// this is ran 1 frame after DoCastBobber because the projectile only exist 1 frame after DoCastBobber is called
		if (shouldCastBobber) {
			// set the bobbers positon to lastBobberPos, this should be a position in water
			Bobber->position = lastBobberPos;
			// clear velocity so it dosen't travel when being teleported to the water
			Bobber->velocity = Microsoft::Xna::Framework::Vector2(0, 0);
			// set shouldCastBobber so this code isn't ran mutiple times
			shouldCastBobber = false;
			return "Set bobber stats...";
		}

		// get bobbser default stats
		if (!Bobber->wet) {
			// if coughtFish is true and this code is reached it mean the rod is reeling in
			if (coughtFish) {
				return "Waiting for real in...";
			}
			return "Bobber is not in liqued.";
		}

		/// the fallowing code was copied from Terraria.Projectile.FishingCheck()
		// create fisher data for retreaving more data
		auto fisher = gcnew Terraria::DataStructures::FishingAttempt();
		fisher->X = (Bobber->Center.X / 16);
		fisher->Y = (Bobber->Center.Y / 16);
		fisher->bobberType = Bobber->type;
		
		// get fishing data using reflection
		array<System::Type^>^ paramTypes = gcnew array<System::Type^>(6) {
			int::typeid, int::typeid,
			System::Boolean::typeid->MakeByRefType(), System::Boolean::typeid->MakeByRefType(),
			System::Int32::typeid->MakeByRefType(), System::Int32::typeid->MakeByRefType()
		};
		auto GetFishingPondState = (Terraria::Projectile::typeid)->GetMethod(
			"GetFishingPondState", 
			System::Reflection::BindingFlags::NonPublic | System::Reflection::BindingFlags::Static, 
			nullptr, paramTypes, nullptr);
		int a = fisher->X; int b = fisher->Y; bool c = false; bool d = false; int e = 0; int f = 0;
		array<Object^>^ parameters = gcnew array<Object^>(6) { a, b, c, d, e, f };
		GetFishingPondState->Invoke(nullptr, parameters);
		fisher->inLava = safe_cast<bool>(parameters[2]);
		fisher->inHoney = safe_cast<bool>(parameters[3]);
		fisher->waterTilesCount = safe_cast<int>(parameters[4]);
		fisher->chumsInWater = safe_cast<int>(parameters[5]);

		// get player data
		fisher->playerFishingConditions = Terraria::Main::player[Bobber->owner]->GetFishingConditions();
		Terraria::DataStructures::PlayerFishingConditions plrFishConds = fisher->playerFishingConditions;

		fisher->CanFishInLava = 
			Terraria::ID::ItemID::Sets::CanFishInLava[plrFishConds.PoleItemType] ||
			Terraria::ID::ItemID::Sets::IsLavaBait[plrFishConds.BaitItemType] ||
			player->accLavaFishing;

		// run some code from Terraria.Projectile.FishingCheck()
		fisher->waterNeededToFish = 300;
		float num = (float)Terraria::Main::maxTilesX / 4200;
		num *= num;
		fisher->atmo = (float)((double)(Bobber->position.Y / 16 - (60 + 10 * num)) / (Terraria::Main::worldSurface / 6.0));
		if ((double)fisher->atmo < 0.25)
			fisher->atmo = 0.25f;
		if (fisher->atmo > 1)
			fisher->atmo = 1;
		fisher->waterNeededToFish = (int)((float)fisher->waterNeededToFish * fisher->atmo);
		fisher->waterQuality = (float)fisher->waterTilesCount / (float)fisher->waterNeededToFish;
		if (fisher->waterQuality < 1)
			fisher->fishingLevel = (int)((float)fisher->fishingLevel * fisher->waterQuality);
		fisher->waterQuality = 1 - fisher->waterQuality;

		// set fish count to max value if the option is enabled
		if (CustomConfigMenu::Option_InstantCatch && Bobber->ai[1] == 0) {
			Bobber->localAI[1] = 660;
		}

		// if compactMode return a more compact version of all the data
		if (compactMode) {
			returnString =
				"Using: [i:" + itemId + "] with [i:" + plrFishConds.BaitItemType + "]\n" +
				"Waiting... " +
				(
					Bobber->ai[1] != 0 
						? "" + "[i:" + Bobber->localAI[1] + "]"
						: "" + Bobber->localAI[1] + " / 660"
				);
		}
		else {
			// add some text stuff
			returnString += "Holding: " + player->HeldItem->Name + " [i:" + itemId + "]";
			returnString += "\nFishing power: " + plrFishConds.FinalFishingLevel;
			returnString += "\nBait type: [i:" + plrFishConds.BaitItemType + "]";
			returnString += "\nChum count: " + fisher->chumsInWater;
			returnString += "\nCan fish in lava: " + fisher->CanFishInLava;
			if (fisher->waterTilesCount < 75) 
				return returnString + "\nNot enough liqued tiles. " + fisher->waterTilesCount + " / " + fisher->waterNeededToFish;
			else
				returnString += "\nLiquid Tile Count: " + fisher->waterTilesCount + " / " + fisher->waterNeededToFish;
			returnString += "\nwaterQuality: " + fisher->waterQuality;
			if (Bobber->ai[1] != 0) {
				returnString += "\nFish on Hook!";
				returnString += "\nFish: [i:" + Bobber->localAI[1] + "]";
			}
			else {
				returnString += "\nWaiting for fish...";
				returnString += "\nFishing count: " + Bobber->localAI[1] + " / 660";
			}
			// used for debgugging // returnString += "\nlocalAI: " + Bobber->localAI[0] + ", " + Bobber->localAI[1] + ", " + Bobber->localAI[2];
			// used for debgugging // returnString += "\nai: " + Bobber->ai[0] + ", " + Bobber->ai[1] + ", " + Bobber->ai[2];
		}


		// if `Bobber->ai[1]` is 0 it means a fish is on the hook
		//  if this is the case catch the fish
		if (CustomConfigMenu::Option_CatchFish && Bobber->ai[1] < 0) {
			// if only catch crate check if the fish is a crate
			if (CustomConfigMenu::Option_OnlyCrates) {
				if (Terraria::ID::ItemID::Sets::IsFishingCrate[(int)Bobber->localAI[1]])
					DoCatchFish(player, Bobber);
			}
			else DoCatchFish(player, Bobber);
		}
		// if drink potions drink potions
		if (CustomConfigMenu::Option_DrinkPotions) {
			int buffCount = player->maxBuffs;
			bool hasCrate = false;
			bool hasFishing = false;
			for (int i = 0; i < buffCount; i++) {
				if (player->buffType[i] == Terraria::ID::BuffID::Crate) {
					hasCrate = true;
				}
				if (player->buffType[i] == Terraria::ID::BuffID::Fishing) {
					hasFishing = true;
				}
			}
			// the fallowing code was borrowed from Terraria.Player.QuickBuff()
			// search inventory for botions
			int itemSlotCount = 58; // only inevntory
			if (player->useVoidBag()) {
				itemSlotCount = 98; // add extra solts of using bag
			}
			for (int i = 0; i < itemSlotCount; i++) {
				Terraria::Item^ item = ((i >= 58) ? player->bank4->item[i - 58] : player->inventory[i]);
				if (item->stack <= 0 || item->type <= 0 || item->buffType <= 0 || item->summon) continue;
				if (
					(item->type == Terraria::ID::ItemID::CratePotion && !hasCrate) ||
					(item->type == Terraria::ID::ItemID::FishingPotion && !hasFishing)
					) {
					int time = item->buffTime;
					if (time == 0) time = 3600;
					player->AddBuff(item->buffType, time, false, false);
					if (item->consumable) {
						item->stack--;
						if (item->stack <= 0) {
							item->TurnToAir(false); // gone, redused to atoms
						}
					}
				}
			}
		}
		// if pause on damage check if the player has i frames (was hit) and pause the game
		if (CustomConfigMenu::Option_PauseHurt && player->immuneTime > 0) {
			Terraria::IngameOptions::Open();
			// turn it off so it dosen't keep pausing the game
			CustomConfigMenu::Option_PauseHurt = false;
		}
		return returnString;
	}

	void DoCatchFish(Terraria::Player^ player, Terraria::Projectile^ bobber) {
		auto ItemCheck_CheckCanUse = (Terraria::Player::typeid)->GetMethod(
			"ItemCheck_CheckCanUse",
			System::Reflection::BindingFlags::NonPublic | System::Reflection::BindingFlags::Instance);
		ItemCheck_CheckCanUse->Invoke(player, gcnew array<Object^>(1) { player->HeldItem });
		// set coughtFish variable to true indicating this program got the fish instead of the player
		coughtFish = true;
		// set lastBobberPos to the bobber positon, this is the bobbers position in water
		//  this i used to set its position in DoCastBobber
		lastBobberPos = bobber->position;
	}
	
	void DoCastBobber(Terraria::Player^ player) {
		// set coughtFish to false so it dosen't keep calling DoCatchFish
		coughtFish = false;
		// fishing rods actualy shoot the projectile instead of spawning one
		auto ItemCheck_StartActualUse = (Terraria::Player::typeid)->GetMethod(
			"ItemCheck_StartActualUse",
			System::Reflection::BindingFlags::NonPublic | System::Reflection::BindingFlags::Instance);
		ItemCheck_StartActualUse->Invoke(player, gcnew array<Object^>(1) { player->HeldItem });
	}

};

ref class CustomMod : cModLoader::ModComponents::Mod {
public:

	CustomUI^ customUI;
	Terraria::UI::UserInterface^ customInterface;

	CustomMod(System::String^ name, System::String^ desc, System::String^ author, System::String^ version) {
		customInterface = gcnew Terraria::UI::UserInterface();
		customUI = gcnew CustomUI();
		customUI->Inistialize();
		customInterface->SetState(customUI);
		CreateGameInterfaceLayer("Custom Layer: Auto Fisher", -1, Terraria::UI::InterfaceScaleType::UI);

		ModName = name;
		ModAuthor = author;
		ModDescription = desc;
		ModVersion = version;

	}
	// simulated code from and before Terraria.Main.UpdateUIStates
	// in tmodloader there is custom code that helps with this
	void OnPostDraw(Microsoft::Xna::Framework::GameTime^ gameTime) override {
		// retain scroll functionality
		int scroll = Terraria::GameInput::PlayerInput::ScrollWheelDeltaForUI;
		Terraria::GameInput::PlayerInput::SetZoom_UI();

		if (customInterface != nullptr) {
			customInterface->Update(gameTime);
		}

		Terraria::GameInput::PlayerInput::SetZoom_Unscaled();
		Terraria::GameInput::PlayerInput::ScrollWheelDeltaForUI = scroll;
	}
	void OnGameInterfaceDraw(Microsoft::Xna::Framework::Graphics::SpriteBatch^ spriteBatch) override {
		customInterface->Draw(spriteBatch, gcnew Microsoft::Xna::Framework::GameTime());
	}

};

extern "C" __declspec(dllexport) void* InjectTerrariaMod(int i) {

	// create mod
	auto mod = gcnew CustomMod(
		"Auto fisher",
		"This is an auto fisher for cModLoader (legacy version)." +
		"It adds a UI element that can be dragged that displays fishing information." +
		"While the player is fishing the bobber will automatically reel, the player collects the fish and the bobber goes back to the water." +
		"There are also some other options like only catch crates (this dose NOT only spawn crates, it just doesn't catch fish) or pause the game when hurt." +
		"There is also a cheat options that automatically sets a fish on the hook, this allows for insane fishing, however I'd say its cheating.", 
		"crawdad105", 
		"v1.0"
	);

	return NULL;
}
