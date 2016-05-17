#include <Arduboy.h>
#include "DefinesImagesAndSounds.h"
#include "SquarioGame.h"
/*
void Item::LoadItem( byte _Type ) {
  Counter = 0;
  Frame = 0;
  Used = false;
  Type = _Type;
  switch ( Type ) {
    case ITEmpty:  MaxFrame = 0; break;
    case ITHammer: MaxFrame = 4; break;
  }
}
void Item::Draw( ) {
  if ( Type == ITEmpty ) return;
  Game->Display->drawBitmap(
    Game->Player.x - Game->CameraX + x,
    Game->Player.y - Game->CameraY + y,
    tile, w, h, WHITE
  );
}
void Item::Cycle( ) {
  if ( Type == ITEmpty ) return;
  if ( Used ) {
    Counter++;
    if ( Counter % 2 == 0 ) Frame++;
    if ( Frame > MaxFrame ) Used = false;
  }
  switch ( Type ) {
    case ITHammer: Hammer( ); break;
  }
}
void Item::Hammer( ) {
  switch ( Frame ) {
    case 0: if ( Game->Player.Mirrored ){ tile = tHammer2; x = 3; }
            else                        { tile = tHammer0; x = -6; }
            w = 11; h = 16; y = -3; break;
    case 1: tile = tHammer1; w = 8; x =  0; y = -4; break; // Hammer Up
    case 2: if ( !Game->Player.Mirrored ) { tile = tHammer2; x = 3; }
            else                          { tile = tHammer0; x = -6; }
            w = 11; y = -3; break;
    case 3: if ( !Game->Player.Mirrored ) { tile = tHammer3;  x =  3; }
            else                          { tile = tHammer3m; x = -6; }
            h =  8; y =  3; break;
    case 4: if ( !Game->Player.Mirrored ) { tile = tHammer4;  x = 4; }
            else                          { tile = tHammer4m; x = -7; }
            h = 16; y =  1; break;
    case 5: if ( !Game->Player.Mirrored ) { tile = tHammer0; x = -6; }
            else                          { tile = tHammer2; x = 3; }
            y = -3; break;
  }
}
void Item::Use( ) {
  if ( Type == ITEmpty ) return;
  if ( !Used ) {
    Used = true;
    Counter = 0;
    Frame = 0;
  }
}
*/
uint8_t Sprite::Width( )    { return pgm_read_byte( SpriteData + SpriteWidth ); }
uint8_t Sprite::Height( )   { return pgm_read_byte( SpriteData + SpriteHeight ); }
uint8_t Sprite::Masks( )    { return pgm_read_byte( SpriteData + SpriteMasks ); }
uint8_t Sprite::MaxFrame( ) { return pgm_read_byte( SpriteData + SpriteMaxFrame ); }
byte    Sprite::Flags( )    { return pgm_read_byte( SpriteData + SpriteFlags ); }
const unsigned char * Sprite::FramePointer ( ) {
  int FrameSize = Height() * Width() / 8;
  return SpriteData + SpriteImageData + ( FrameSize * currentFrame );
}
const unsigned char * Sprite::MaskPointer ( ) {
  int FrameSize = Height() * Width() / 8;
  return SpriteData + SpriteImageData + ( FrameSize * MaxFrame() ) + ( FrameSize * currentFrame );
}
void Sprite::Cycle( ) {
/*if ( vx ) {
    currentFrame++;
    if (currentFrame > maxFrame) currentFrame = 0;
  }
  else currentFrame = 0;*/
}
void Sprite::LoadSprite( const unsigned char * DataPointer, int tX, int tY ) {
  SpriteData = DataPointer;
  x = tX; y = tY;
  vx = 0; vy = 0;
  Mirrored = false;
  currentFrame = 0;
/*maxFrame = 0;
  flags = 0;
  switch ( type ) {
    case STSquario:
      w = 8; h = 8;
      tile = tSmallSquario;
      mask = tSmallSquarioMask;
      break;
    case STBigSquario:
      w = 8; h = 16;
      tile = tBigSquario;
      mask = tBigSquarioMask;
      break;
    case STTriangleo:
      w = 8; h = 8;
      tile = tTriangleo;
      mask = tTriangleoMask;
      break;
    case STStarmano:
      w = 8; h = 8;
      tile = tStarmano;
      mask = tStarmano;
      break;
    case STBolt:
      w = 16; h = 16;
      tile = tBolt;
      mask = tBolt;
      flags = 1;
      break;
    case STSmileo:
      w = 8; h = 8;
      tile = tSmileo;
      mask = tSmileo;
      break;
    case STMushroom:
      w = 8; h = 8;
      tile = tMushroom;
      mask = tMushroom;
      break;
  } */
}
void Sprite::ClearSprite( ) {
  SpriteData = NULL;
  x = -1;
  y = -1;
  vx = 0;
  vy = 0;
  currentFrame = 0;
  Mirrored = false;
}
bool Sprite::IsIn ( int tX, int tY ) {
  if ( tX >= x &&
       tX <= RightX() &&
       tY >= y &&
       tY <= BottomY() ) return true;
  return false;
}
bool Sprite::IsInTopHalf ( int tX, int tY ) {
  if ( tX >= x &&
       tX <= RightX() &&
       tY >= y &&
       tY <= (y+(Height()/2)) ) return true;
  return false;
}
byte Sprite::Collide( int tX, int tY ) {
  int nX = tX / TileSize;
  int nY = tY / TileSize;
  if ( Game->Level.CheckTile( nX, nY ) ) return 0xFF;
  return Game->Level.CheckObject( nX, nY );
}
bool Sprite::Falling( ) {
  return !CollisionCheckY( Down );
}
void Sprite::Gravity( ) {
  if ( vy < 6 ) vy++;
}
bool Sprite::CollisionCheckX( byte direction ) {
  for ( uint8_t tY = 0; tY < ( Height() / TileSize ); tY++ ) {
    switch ( direction ) {
      case Left:
        if ( Collide( x-1, y+(tY*TileSize) ) || Collide( x-1, y+((tY+1)*TileSize)-1 ) ) return true;
        break;
      case Right:
        if ( Collide( x + Width(), y+(tY*TileSize) ) || Collide( x+Width(), y+((tY+1)*TileSize)-1 ) ) return true;
        break;
    }
  }
  return false;
}
bool Sprite::CollisionCheckY( byte direction ) {
  for ( uint8_t tX = 0; tX < (Width()/TileSize); tX++ ) {
    switch ( direction ) {
      case Up:
        if ( Collide( x+(tX*TileSize), y-1 ) || Collide( x+((tX+1)*TileSize)-1, y-1 ) ) return true;
        break;
      case Down:
        if ( Collide( x+(tX*TileSize), y+Height() ) || Collide( x+((tX+1)*TileSize)-1, y+Height() ) ) return true;
        break;
    }
  }
  return false;
}
void Sprite::Move( ) {
  if ( Flags() & 1 ) {
    x += vx;
    y += vy;
    return;
  }
  if ( vy == 0 ) { // Start fall
    if ( Falling( ) ) Gravity( );
  }
  if ( vy > 0 ) { // Down
    for ( int a = 0; a < vy; a++ ) {
      if ( Falling( ) ) {
        y++;
      }
      else { vy = 0; break; }
    }
    Gravity( );
  }
  if ( vy < 0 ) { // Up
    Gravity( );
    for ( int a = 0; a > vy; a-- ) {
      if ( CollisionCheckY(Up) ) { HeadCollision(); vy = 0; break; }
      else y--;
    }
  }
  if ( vx > 0 ) { // Right
    for ( int a = 0; a < vx; a++ ){
    if ( RightX() > Game->Level.MaxXPixel() ) break;
      if ( CollisionCheckX(Right) ) break;
      else x++;
    }
//  if ( !Game->MainHand.Used ) Mirrored = false;
  }
  if ( vx < 0 ) { // Left
    for ( int a = 0; a > vx; a-- ){
      if ( x < Game->Level.MinXPixel() + 1 ) break;
      if ( CollisionCheckX(Left) ) break;
      else x--;
    }
//  if ( !Game->MainHand.Used ) Mirrored = true;
  }
}
bool Sprite::Jump( ) {
  if ( CollisionCheckY( Down ) ) { vy = -8; return true; }
  return false;
}
void Sprite::Duck( ) {
  if ( Collide( x, y+Height()+1 ) == STPipeCapLeft && Collide( RightX() , y+Height()+1 ) == STPipeCapRight ) {
    Game->Event = ETPipeDrop;
    Game->EventCounter = 0;
  }
}
void Sprite::HeadCollision( ) {
  if ( Flags() & 0b10 ) return;
  byte LeftCheck = Collide( x, y-1 );
  byte RightCheck = Collide( RightX(), y-1 );
  
  if ( LeftCheck == STQBlock || LeftCheck == STMushBlock ) {
    Game->Level.HandleObject( x / TileSize, (y-1) / TileSize );
  }
  if ( RightCheck == STQBlock || RightCheck == STMushBlock ) {
    Game->Level.HandleObject( RightX() / TileSize, (y-1) / TileSize );
  }
  
}
void Sprite::Draw( ) {
  if ( Masks() ) Game->Display->drawBitmap(
    x - Game->CameraX,
    y - Game->CameraY,
    MaskPointer( ), Width(), Height(), BLACK );
  Game->Display->drawBitmap(
    x - Game->CameraX,
    y - Game->CameraY,
    FramePointer( ),
    Width(), Height(), WHITE );
}
int Sprite::RightX( ) { return x+Width()-1; }
int Sprite::BottomY( ) { return y+Height()-1; }

uint8_t AISprite::Speed( )        { return pgm_read_byte( SpriteData + SpriteSpeed ); }
byte    AISprite::Intelligence( ) { return pgm_read_byte( SpriteData + SpriteIntelligence ); }
void AISprite::Activate( const unsigned char * DataPointer, int tX, int tY ) {
  SpriteData = DataPointer;
  Active = true;
  Facing = Left;
  LoadSprite( DataPointer, tX*TileSize, tY*TileSize );
  if ( DataPointer == BoltSprite ) {
    vx = -4;
    vy = 2;
  }
  Think();
}
void AISprite::Deactivate( ) {
  SpriteData = NULL;
  Active = false;
  Facing = 0;
  ClearSprite();
}
void AISprite::Think( ) {
  if   ( Intelligence() & 0b00000100 ) {
    Seek();
    if ( Intelligence() & 0b00001000 ) DetectJump();
  }
  else {
    if ( Intelligence() & 0b00000001 ) DetectWall();
    if ( Intelligence() & 0b00000010 ) DetectGap();
  }
  Move();
}
void AISprite::Seek( ) {
  if ( Game->Player.x + Game->Player.Width() < x ) {
    Facing = Left;
    vx = Speed() * -1;
  }
  if ( Game->Player.x > x + Width() ) {
    Facing = Right;
    vx = Speed();
  }
}
void AISprite::DetectJump( ) {
  if ( CollisionCheckX( Facing ) ) Jump();
  if ( Facing == Left ) {
    if ( !Collide( x-1, (RoomHeight*TileSize) - 1 ) ) Jump();
  }
  if ( Facing == Right ) {
    if ( !Collide( RightX()+1, (RoomHeight*TileSize) - 1 ) ) Jump();
  }
}
void AISprite::DetectWall( ) {
  if ( Facing == Left ) {
    if ( CollisionCheckX(Left) ) { Facing = Right; vx = 0; return; }
    else vx = Speed() * -1;
  }
  if ( Facing == Right ) {
    if ( CollisionCheckX(Right) ) { Facing = Left; vx = 0; return; }
    else vx = Speed();
  }
}
void AISprite::DetectGap( ) {
  if ( Facing == Left ) {
    if ( !Collide( x-1, BottomY()+1 ) ) { Facing = Right; vx = 0; return; }
    else vx = Speed() * -1;
  }
  if ( Facing == Right ) {
    if ( !Collide( RightX()+1, BottomY() + 1 ) ) { Facing = Left; vx = 0; return; }
    else vx = Speed();
  }
}

void Room::ClearRoom( ) {
  for ( int a = 0; a < RoomBytes; a++ ) data[a] = 0;
}
void Room::SetTile( int x, int y ) {
  if ( x > RoomWidth || y > RoomHeight ) return;
  int Index = ( RoomHeight/8 ) * x;
  if ( y < 8 ) data[ Index ]   |= 1 << y;
  else         data[ Index+1 ] |= 1 << (y - 8);
}
bool Room::ReadTile( int x, int y ) {
  if ( x > RoomWidth || y > RoomHeight ) return false;
  int Index = ( RoomHeight/8 ) * x;
  if ( y < 8 ) {
    byte Mask = 1 << y;
    if ( data[ Index ] & Mask ) return true;
  }
  else {
    byte Mask = 1 << (y - 8);
    if ( data[ Index+1 ] & Mask ) return true;
  }
  return false;
}

void Map::GenerateRoom( int RoomNum ) {
  randomSeed( Game->Seeds[ ( Game->MapNumber + RoomNum ) % GameSeeds ] * Game->MapNumber + RoomNum );
  rooms[RoomNum%MapRooms].ClearRoom();
  uint8_t Ceiling = 0;
  uint8_t Floor = random(RoomHeight-3,RoomHeight);
  int Gap = 0;
  int tSpawnBarrier = RoomNum*RoomWidth;
  if ( !RoomNum ) AddPipe( 1, Floor-2 );
  for ( int x = 0; x < RoomWidth; x++ ) {
    if ( !Gap ) {
      for ( int b = Floor; b < RoomHeight; b++ ) {
        rooms[RoomNum % MapRooms].SetTile(x, b);
      }
      if ( RoomNum ) {
        if ( !random(10) ) Gap = random(2,5);
        if ( tSpawnBarrier > SpawnBarrier ) {
          if ( !random( 6 ) ) {
            uint8_t MobSelector = random(20);
            if      ( MobSelector < 10 ) Game->AddMob( TriangleoSprite,  tSpawnBarrier + x, Floor - 2 );
            else if ( MobSelector < 16 ) Game->AddMob( SmileoSprite,     tSpawnBarrier + x, Floor - 2 );
            else if ( MobSelector < 19 ) Game->AddMob( StarmanoSprite,   tSpawnBarrier + x, Floor - 2 );
            else                         Game->AddMob( BoltSprite,       tSpawnBarrier + x, Ceiling + 2 );
          }
          if ( !random( 16 ) && !Gap ) {
            int y = random( Floor - 7, Floor - 3 );
            if ( !random(4) ) AddObject ( STMushBlock, tSpawnBarrier + x, y );
            else              AddObject ( STQBlock, tSpawnBarrier + x, y );
          }
        }
        if ( !random(5)  ) {
          if ( !random(1) && Floor < RoomHeight-1 ) Floor++;
          else Floor--;
        }
      }
    }
    else Gap--;
  }
//if ( RoomNum == LastRoom ) AddPipe( MaxXTile() - 2, Ceiling-Floor-2 );
  if ( tSpawnBarrier > SpawnBarrier ) SpawnBarrier = tSpawnBarrier;
}
void Map::AddPipe( int x, int y ) {
  AddObject( STPipeCapLeft,  x,   y );
  AddObject( STPipeCapRight, x+1, y );
  AddObject( STPipeLeft,     x,   y+1 );
  AddObject( STPipeRight,    x+1, y+1 );
}
void Map::AddObject( byte type, int tX, int tY ) {
  if ( CheckObject( tX, tY ) ) return;
  else {
    objects[ObjectIndex].x = tX;
    objects[ObjectIndex].y = tY;
    objects[ObjectIndex].type = type;
    switch (type) {
      case STQBlock: break;
      case STPipeCapLeft: break;
      case STPipeCapRight: break;
      case STPipeLeft: break;
      case STPipeRight: break;
    }
    ObjectIndex++;
    if ( ObjectIndex == MapObjects ) ObjectIndex = 0;
  }
}
void Map::HandleObject( int x, int y ) {
  for ( int a = 0; a < MapObjects; a++ ) {
    if ( objects[a].x == x && objects[a].y == y ) {
      switch ( objects[a].type ) {
        case STQBlock: Game->SFX = SFXCoin; Game->Coins++; objects[a].type = STBQBlock; break;
        case STMushBlock:
          Game->AddMob ( MushroomSprite, x, y-1 );
          objects[a].type = STBQBlock; break;
      }
    }
  }
}
void Map::RemoveObject( int x, int y ) {
  for ( int a = 0; a < MapObjects; a++ ) {
    if ( objects[a].x == x && objects[a].y == y ) {
      objects[a].y = -1;
      return;
    }
  }
}
void Map::NewMap( ) {
  // Reset Variables
  ObjectIndex = 0; LastLoadLocation = 0; FirstRoom = 0; SpawnBarrier = 0;
  for ( uint8_t a = 0; a < MapObjects; a++ ) objects[a].y = -1;
  for ( uint8_t a = 0; a < SpriteCap; a++ ) Game->Mobs[a].Deactivate();

  // Seed for level length
  randomSeed( Game->Seeds[ Game->MapNumber % GameSeeds ] * Game->MapNumber );
  int LowEnd = 7 + random( Game->MapNumber );
  int HighEnd = random( LowEnd, LowEnd + Game->MapNumber );
  LastRoom = 255;//random( LowEnd, HighEnd );

  for ( uint8_t a = 0; a < MapRooms; a++ ) GenerateRoom(a);
}
void Map::LoadMap( ) {
  int RoomBeforePlayerIsIn = ( Game->Player.x / (RoomWidth*TileSize) ) - 1;
  if (RoomBeforePlayerIsIn < 0) RoomBeforePlayerIsIn = 0;
  if (RoomBeforePlayerIsIn > FirstRoom) {
    FirstRoom = RoomBeforePlayerIsIn;
    GenerateRoom( RoomBeforePlayerIsIn + MapRooms - 1 );
  }
}
bool Map::CheckTile( int x, int y ) {
  int room = ( x / RoomWidth ) % MapRooms;
  return rooms[room].ReadTile ( x % RoomWidth, y );
}
byte Map::CheckObject ( int x, int y ) {
  if ( y < 0 ) return 0;
  for ( int a = 0; a < MapObjects; a++ ) {
    if ( ( objects[a].x == x ) && ( objects[a].y == y ) ) return objects[a].type;
  }
  return 0;
}
int Map::MinXPixel() { return FirstRoom * RoomWidth * TileSize; }
int Map::MaxXPixel() { return ( ( LastRoom + 1 ) * RoomWidth * TileSize ) - 1; }
int Map::MaxYPixel() { return ( ( MapHeight + 1 ) * RoomHeight * TileSize ) - 1; }
int Map::MinXTile() { return FirstRoom * RoomWidth; }
int Map::MaxXTile() { return ( ( LastRoom + 1 ) * RoomWidth ) - 1; }
int Map::MaxYTile() { return ( ( MapHeight + 1 ) * RoomHeight ) - 1; }

SquarioGame::SquarioGame( Arduboy * display ) {
  Display = display;
  Player.Game = this;
  Level.Game = this;
//MainHand.Game = this;
  for ( uint8_t a = 0; a < SpriteCap; a++ ) Mobs[a].Game = this;
}
void SquarioGame::NewGame( ) {
  Score = 0;
  Coins = 0;
  Lives = 1;
  MapNumber = 1;
  Event = ETPlaying;
  BeginMap( );
}
void SquarioGame::ProcessButtons( ) {
  uint8_t MaxSpeed = ButtonState[ButtonRun] ? 4 : 3;
//if ( ButtonState[ButtonRun] ) MainHand.Use( );
  if ( !ButtonState[ButtonLeft] && !ButtonState[ButtonRight] ) {
    if ( Player.vx > 0 ) Player.vx--;
    if ( Player.vx < 0 ) Player.vx++;
  }
  if ( ButtonState[ButtonLeft] )  { Player.vx--; if ( Player.vx < MaxSpeed * -1 ) Player.vx = MaxSpeed * -1; }
  if ( ButtonState[ButtonRight] ) { Player.vx++; if ( Player.vx > MaxSpeed ) Player.vx = MaxSpeed; }
  if ( ButtonState[ButtonJump] ) {
    if ( Player.Jump() ) SFX = SFXJump;
  }
  if ( ButtonState[ButtonDown] ) Player.Duck();
}
void SquarioGame::AdjustCamera( ) {
  int MaxX = Level.MaxXPixel() - WIDTH;
  int MaxY = Level.MaxYPixel() - HEIGHT;
  int MinX = Level.MinXPixel();
  CameraX = Player.x - (WIDTH/2); // Center X on player
  CameraY = Player.y - 8;

  // Constrain for map edges
  if ( CameraY > MaxY ) CameraY = MaxY;
  if ( CameraX > MaxX ) CameraX = MaxX;
  if ( CameraX < MinX ) CameraX = MinX;

  // Reload map data
//  if ( Level.LastLoadLocation > Player.x - 128 ) {
//    Level.LastLoadLocation = Player.x;
    Level.LoadMap();
//  }
//  int lastChange = Level.LastLoadLocation - Player.x;
//  if ( lastChange > 128 || lastChange < -128 ) Level.LoadMap();
}
void SquarioGame::Cycle( ) {
  bool Death = false;
  int MapPixelHeight = Level.MaxYPixel();
  ProcessButtons();
  if ( Event == ETPipeDrop && EventCounter < Player.Height()) {
    Player.y++;
    EventCounter++;
  }
  else if ( Event == ETDeath ) {
    if ( EventCounter > 25 ) Player.y--;
    else Player.y+=2;
    if ( Player.y > MapPixelHeight ) EventCounter = 0;
    else EventCounter--;
  }
  else if ( Event == ETPlaying ) {
    Player.Move();
    Player.Cycle();
//  MainHand.Cycle();
    AdjustCamera();
  }
  for ( uint8_t a = 0; a < SpriteCap; a++ ) {
    if ( Mobs[a].Active ) {
      Mobs[a].Think();
      Mobs[a].Cycle();
      if ( Mobs[a].y > MapPixelHeight ) Mobs[a].Deactivate();
      else if ( Mobs[a].IsInTopHalf ( Player.x, Player.BottomY() ) ||
                Mobs[a].IsInTopHalf ( Player.RightX(), Player.BottomY() ) ) {
        if ( Mobs[a].SpriteData == MushroomSprite ) {
          Mobs[a].Deactivate();
          Score += POINTSMushroom;
          if ( Player.SpriteData == SmallSquarioSprite ) {
            Player.LoadSprite( BigSquarioSprite, Player.x, Player.y-8 );
            SFX = SFXMushroom;
          }
          else Coins++;
        }
        else {
          Mobs[a].Deactivate();
          Score += POINTSKill;
          SFX = SFXHit;
          if ( ButtonState[ ButtonJump ] ) { Player.vy = -10; }
          else { Player.vy = -4; }
        }
      }
      else if ( Player.IsIn( Mobs[a].x        , Mobs[a].y ) ||
                Player.IsIn( Mobs[a].RightX() , Mobs[a].y ) ||
                Player.IsIn( Mobs[a].x        , Mobs[a].BottomY() ) ||
                Player.IsIn( Mobs[a].RightX() , Mobs[a].BottomY() ) ) {
        if ( Mobs[a].SpriteData == MushroomSprite ) {
          Mobs[a].Deactivate();
          Score += POINTSMushroom;
          if ( Player.SpriteData == SmallSquarioSprite ) {
            Player.LoadSprite( BigSquarioSprite, Player.x, Player.y-8 );
            SFX = SFXMushroom;
          }
          else Coins++;
        }
        else {
          if ( !EventCounter && Player.Height() == TileSize ) {
            Event = ETDeath;
            EventCounter = 30;
          }
          if ( !EventCounter && Player.Height() > TileSize ) {
            Player.LoadSprite( SmallSquarioSprite, Player.x, Player.y+8 );
            SFX = SFXHit;
            EventCounter = 30;
          }
        }
      }
    }
  }
  if ( Event == ETPlaying && Player.y > MapPixelHeight ) { Event = ETDeath; EventCounter = 25; }
  if ( Event == ETDeath && !EventCounter ) Die();
  if ( Event == ETPlaying && EventCounter ) EventCounter--;
  if ( Event == ETPipeDrop && EventCounter == Player.Height() ) {
    MapNumber++;
    BeginMap( );
  }
}
void SquarioGame::Die( ) {
  Draw();
  Event = ETOff;
  Display->drawRect(16,8,96,48, WHITE); // Box border
  Display->fillRect(17,9,94,46, BLACK); // Black out the inside
  Display->drawSlowXYBitmap(30,12,gameover,72,14,1);
  
  Display->setCursor(26,29); Display->print("Score");
  Display->setCursor(20,37); Display->print("+Dist.");
  Display->setCursor(26,45); Display->print("Total");
  
  Display->setCursor(96-getTextOffset(Score),29); Display->print(Score);
  Display->display();
  delay(250);
  while (!Display->buttonsState());
  
  int DistancePoints = Player.x / TileSize;
  Display->setCursor(96-getTextOffset(DistancePoints),37); Display->print(DistancePoints);
  Display->display();
  delay(250);
  while (!Display->buttonsState());
  
  Score += DistancePoints;
  Display->setCursor(96-getTextOffset(Score),45); Display->print(Score);
  Display->display();
  delay(250);
  while (!Display->buttonsState());

  
//BeginMap( );
}
void SquarioGame::BeginMap( ) {
  SFX = 0;
  Event = ETPlaying;
  Player.LoadSprite( SmallSquarioSprite, 10, 0 );
  // MainHand.LoadItem( ITHammer );
  Level.NewMap( );
  while ( Player.Falling() ) Player.Move();
  AdjustCamera( );
}
void SquarioGame::DrawMobs( ) {
  for ( uint8_t a = 0; a < SpriteCap; a++ ) {
    if ( Mobs[a].Active ) {
      if ( Mobs[a].RightX() - CameraX > 0 && Mobs[a].x - CameraX < 128 ) Mobs[a].Draw();
    }
  }
}
void SquarioGame::DrawMap( ) {
  int mountainOffset = ( CameraX / 4 ) % 64;
  int mountainYOffset = ( CameraY / 12 ) - 4;
  Display->drawBitmap(  0 - mountainOffset, mountainYOffset,OverworldBG,64,16,1);
	Display->drawBitmap( 64 - mountainOffset, mountainYOffset,OverworldBG,64,16,1);
	Display->drawBitmap(128 - mountainOffset, mountainYOffset,OverworldBG,64,16,1);
  for ( int x = CameraX / TileSize; x < (CameraX / TileSize) + 17; x++ ) {
    for ( int y = CameraY / TileSize; y < (CameraY / TileSize) + 9; y++ ) {
      if ( Level.CheckTile( x, y ) ) {
        Display->drawBitmap( x*TileSize - CameraX, y*TileSize - CameraY, tMask, TileSize, TileSize, BLACK );
        Display->drawBitmap( x*TileSize - CameraX, y*TileSize - CameraY, tBrick, TileSize, TileSize, WHITE );
      }
      else {
        const unsigned char * tempTile = 0;
        switch ( Level.CheckObject(x,y) ) {
          case STMushBlock:
          case STQBlock:        tempTile = tQBlock; break;
          case STBQBlock:       tempTile = tBQBlock; break;
          case STPipeCapLeft:   tempTile = tPipeCapLeft; break;
          case STPipeCapRight:  tempTile = tPipeCapRight; break;
          case STPipeLeft:      tempTile = tPipeLeft; break;
          case STPipeRight:     tempTile = tPipeRight; break;
        }
        if ( tempTile ) {
          Display->drawBitmap( x*TileSize - CameraX, y*TileSize - CameraY, tMask, TileSize, TileSize, BLACK );
          Display->drawBitmap( x*TileSize - CameraX, y*TileSize - CameraY, tempTile, TileSize, TileSize, WHITE );
        }
      }
    }
  }
}
void SquarioGame::Draw() {
  switch ( Event ) {
    case ETDeath:
    case ETPlaying:   DrawMap(); DrawMobs(); 
                      if ( !(EventCounter % 2) ) { Player.Draw(); /* MainHand.Draw(); */ }
                      break;
    case ETPipeDrop:  Player.Draw(); /* MainHand.Draw(); */ DrawMap(); DrawMobs(); break;
  }
}
void SquarioGame::AddMob( const unsigned char * DataPointer, int x, int y ) {
  int Distances[ SpriteCap ];
  for ( uint8_t a = 0; a < SpriteCap; a++ ) {
    if ( !Mobs[a].Active ) { Mobs[a].Activate( DataPointer, x, y ); return; }
    Distances[a] = Player.x - Mobs[a].x;
    if ( Distances[a] < 0 ) Distances[a] *= -1;
  }
  uint8_t Distance = Distances[0];
  uint8_t Candidate = 0;
  for ( uint8_t a = 1; a < SpriteCap; a++ ) {
    if ( Distances[a] > Distance ) {
      Candidate = a;
      Distance = Distances[a];
    }
  }
  Mobs[Candidate].Activate( DataPointer, x, y );
}
void SquarioGame::ButtonPress( uint8_t pButton ) {
  if ( ButtonState[ pButton ] ) return;
  ButtonState[ pButton ] = true;
}
void SquarioGame::ButtonRelease( uint8_t pButton ) {
  if ( !ButtonState[ pButton ] ) return;
  ButtonState[ pButton ] = false;
}
int SquarioGame::getTextOffset(int s) {
  if (s > 9999) { return 20; }
  if (s > 999) { return 15; }
  if (s > 99) { return 10; }
  if (s > 9) { return 5; }
  return 0;
}


