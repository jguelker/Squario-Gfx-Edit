#ifndef SQUARIO_h
#define SQUARIO_h
#include <Arduboy.h>
#include "DefinesImagesAndSounds.h"

enum SquarioButtons {
  ButtonLeft = 0,
  ButtonUp,
  ButtonRight,
  ButtonDown,
  ButtonJump,
  ButtonRun,
  NumButtons
};
enum Directions {
  Up,
  Down,
  Left,
  Right
};
enum ObjectTypes {
  STSquario = 0, // Sprites
  STBigSquario,
  STTriangleo,
  STStarmano,
  STSmileo,
  STBolt,
  
  STMushroom,

  STQBlock,  // Interactive Tiles
  STBQBlock,
  STMushBlock,
  STBricks,
  STPipeCapLeft,
  STPipeCapRight,
  STPipeLeft,
  STPipeRight,
  
  NumObjectTypes
};
enum MapTypes {
  MTMaze = 0,
  MTHorizontal,
  MTVertical,
  MTFalling,
  NumMapTypes
};
enum ItemTypes {
  ITEmpty,
  ITHammer,
  ITAxe,
  ITGun,
  NumItemTypes
};
enum EventTypes {
  ETOff = 0,
  ETPlaying,
  ETDeath,
  ETPipeDrop,
  ETNum
};
enum Sounds {
  SFXSilence,
  SFXJump,
  SFXMushroom,
  SFXHit,
  SFXCoin
};

class Room;
class Map;
//class Item;
class Sprite;
class AISprite;
class SquarioGame;
/*
class Item {
  public:
    void LoadItem( byte _Type );
    void Draw( );
    void Cycle( );
    void Use( );
    
    void Hammer( );
    
    bool                  Used;
    SquarioGame         * Game;
    const unsigned char * tile;
    const unsigned char * mask;
    byte                  Type;
    uint8_t               Frame, MaxFrame, w, h;
    int                   x, y, Counter;
};
*/
class Sprite {
  public:
    void LoadSprite( const unsigned char * DataPointer, int tX, int tY );
    void ClearSprite( );
    bool IsIn ( int tX, int tY );
    bool IsInTopHalf ( int tX, int tY );
    byte Collide( int tX, int tY );
    bool CollisionCheckX( byte direction );
    bool CollisionCheckY( byte direction );
    void HeadCollision( );
    bool Falling( );
    void Gravity( );
    void Move( );
    bool Jump( );
    void Duck( );
    void Cycle( );
    void Draw( );
    
    uint8_t Width( );
    uint8_t Height( );
    uint8_t Masks( );
    uint8_t MaxFrame( );
    byte Flags( );
    const unsigned char * FramePointer( );
    const unsigned char * MaskPointer( );
    int RightX( );
    int BottomY( );

    SquarioGame         * Game;
    const unsigned char * SpriteData;
    int                   x, y, vx, vy;
    uint8_t               currentFrame;
    bool                  Mirrored;

};
class AISprite : public Sprite {
  public:
    void Activate( const unsigned char * DataPointer, int tX, int tY );
    void Deactivate( );
    void Think( );
    void Seek( );
    void DetectJump( );
    void DetectWall( );
    void DetectGap( );
    
    uint8_t Speed( );
    byte Intelligence( );
    bool                  Active;
    byte                  Facing;
};
class InteractiveObject {
  public:
    int x, y;
    byte type;
};
class Room {
  public:
    void ClearRoom( );
    void SetTile( int x, int y );
    bool ReadTile( int x, int y );
    byte         data[ RoomBytes ];
};
class Map {
  public:
    void NewMap( );
    void LoadMap( );
    void GenerateRoom( int RoomNum );
    void AddObject( byte type, int x, int y );
    void HandleObject ( int x, int y );
    void RemoveObject( int x, int y );
    byte CheckObject( int x, int y );
    bool CheckTile( int x, int y );
    void AddPipe( int x, int y );

    int MinXPixel();
    int MaxXPixel();
    int MaxYPixel();
    int MinXTile();
    int MaxXTile();
    int MaxYTile();
    
    SquarioGame         * Game;
    Room                  rooms[MapRooms];
    InteractiveObject     objects[MapObjects];
    int                   ObjectIndex;
    int                   RandomChance;
    int                   FirstRoom, LastRoom, MapHeight, LastLoadLocation, SpawnBarrier;
};
class SquarioGame {
  public:
    SquarioGame( Arduboy * display );
    void NewGame( );
    void Cycle( );
    void Draw( );
    void Die( );
    void BeginMap( );
    void DrawMap( );
    void DrawMobs( );
    void AddMob( const unsigned char * DataPointer, int x, int y );
    void AdjustCamera( );
    void ProcessButtons( );
    void ButtonPress( uint8_t pButton );
    void ButtonRelease( uint8_t pButton );
    bool ButtonState[ NumButtons ];
    int getTextOffset( int s );
    
    Arduboy             * Display;
    Sprite                Player;
//  Item                  MainHand;
    AISprite              Mobs[ SpriteCap ];
    Map                   Level;

    unsigned int          Score;
    int                   Coins, Lives, MapNumber;
    byte                  Inventory;
    int                   CameraX, CameraY;
    byte                  Event;
    int                   EventCounter;
    byte                  SFX;
    byte                  Seeds[ GameSeeds ];
};


#endif
