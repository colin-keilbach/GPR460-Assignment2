//I take care of rendering things!

#include <SDL_ttf.h>

struct TextRenderer
{
	string mText;
	Vector3 mPosition;
	Vector3 mColor;
	float mTime;
};

class HUD
{
public:

	static void StaticInit();
	static std::unique_ptr< HUD >	sInstance;

	void Render();

	void			SetPlayerHealth( int inHealth )	{ mHealth = inHealth; }

	void			AddTextRenderer( const string& inStr, const Vector3& origin, const Vector3& inColor, int inTime = 60 );


private:

	HUD();

	void	RenderTurnNumber();
	void	RenderBandWidth();
	void	RenderScoreBoard();
	void	RenderHealth();
	void	RenderCountdown();
	void	RenderTextList();

	void	RenderText( const string& inStr, const Vector3& origin, const Vector3& inColor );

	Vector3										mBandwidthOrigin;
	Vector3										mRoundTripTimeOrigin;
	Vector3										mScoreBoardOrigin;
	Vector3										mScoreOffset;
	Vector3										mHealthOffset;
	Vector3										mTimeOrigin;
	SDL_Rect									mViewTransform;

	std::vector<TextRenderer> mTextList;

	TTF_Font*									mFont;
	int											mHealth;
};