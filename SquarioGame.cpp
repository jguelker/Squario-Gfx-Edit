#include <Arduboy.h>
#include "DefinesImagesAndSounds.h"
#include "SquarioGame.h"

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
void Sprite::LoadSprite( const unsigned char * DataPointer, int tX, int tY ) {
  SpriteData = DataPointer;
  x = tX; y = tY;
  vx = 0; vy = 0;
  Mirrored = false;
  currentFrame = 0;
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
bool Sprite::GetPixelAbsolute( int tX, int tY ) {
  int RelativeX = tX - x;
  int RelativeY = tY - y;
  if ( RelativeX < 0 || RelativeY < 0 || RelativeX > Width() || RelativeY > Height() ) return false;
  uint8_t row = RelativeY / 8;
  uint8_t offset = RelativeY % 8;
  byte selection = pgm_read_byte( MaskPointer() + ( row * Width() ) + RelativeX );
  return ( selection & ( 1 << offset ) );
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
  if ( x < 64 ) return;
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
  uint8_t Floor = random(RoomHeight-3,RoomHeight);
  uint8_t Ceiling = ( !( Game->MapNumber % 2 ) ) ? 8 : 0;
  int Gap = 0;
  int tSpawnBarrier = RoomNum*RoomWidth;
  if ( !RoomNum ) AddPipe( 1, Floor-2 );
  for ( int x = 0; x < RoomWidth; x++ ) {
    if ( Ceiling ) rooms[RoomNum % MapRooms].SetTile(x, Ceiling);
    if ( !Gap ) {
      for ( int b = Floor; b < RoomHeight; b++ ) {
        rooms[RoomNum % MapRooms].SetTile(x, b);
      }
      if ( RoomNum && ( RoomNum < LastRoom ) ) {
        if ( !random(10) ) { 
          Gap = random(2,5);
          if ( Ceiling ) Gap--;
        }
        else if ( !random(5)  ) {
          if ( !random(1) && Floor < RoomHeight-1 ) Floor++;
          else Floor--;
        }
        if ( tSpawnBarrier > SpawnBarrier ) {
          if ( !random( 8 ) ) {
            uint8_t MobSelector = random(20);
            if      ( MobSelector < 10 ) Game->AddMob( TriangleoSprite, tSpawnBarrier + x, Floor - 2 );
            else if ( MobSelector < 16 ) Game->AddMob( SmileoSprite, tSpawnBarrier + x, Floor - 2 );
            else if ( MobSelector < 19 ) {
              if ( RoomNum > 8 ) Game->AddMob( StarmanoSprite, tSpawnBarrier + x, Floor - 2 );
            }
            else { 
              if ( RoomNum > 8 ) Game->AddMob( BoltSprite, tSpawnBarrier + x, 2 );
            }
          }
          if ( !random( 16 ) && !Gap && Floor > Ceiling + 5 ) {
            int y = random( max( Floor - 7, Ceiling + 2 ), Floor - 3 );
            if ( !random(4) ) AddObject ( STMushBlock, tSpawnBarrier + x, y );
            else              AddObject ( STQBlock, tSpawnBarrier + x, y );
          }
        }
      }
    }
    else Gap--;
  }
  if ( RoomNum == LastRoom ) AddPipe( MaxXTile() - 2, Floor-2 );
  if ( tSpawnBarrier > SpawnBarrier ) SpawnBarrier = tSpawnBarrier;
}
void Map::AddPipe( int x, int y ) {
  AddObject( STPipeCapLeft,  x,   y );
  AddObject( STPipeCapRight, x+1, y );
  for ( uint8_t a = y+1; a < RoomHeight; a++ ) {
    AddObject( STPipeLeft,     x,   a );
    AddObject( STPipeRight,    x+1, a );
  }
}
void Map::AddObject( byte type, int tX, int tY ) {
  if ( CheckObject( tX, tY ) ) return;
  else {
    objects[ObjectIndex].x = tX;
    objects[ObjectIndex].y = tY;
    objects[ObjectIndex].type = type;
    ObjectIndex++;
    if ( ObjectIndex == MapObjects ) ObjectIndex = 0;
  }
}
void Map::HandleObject( int x, int y ) {
  for ( int a = 0; a < MapObjects; a++ ) {
    if ( objects[a].x == x && objects[a].y == y ) {
      switch ( objects[a].type ) {
        case STQBlock: 
          Game->Coins++;
          if ( !( Game->Coins % 20 ) ) {
            Game->Lives++;
            Game->SFX = SFX_Life;
          }
          else Game->SFX = SFX_Coin;
          objects[a].type = STBQBlock;
          break;
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
  int LowEnd = MinLevelWidth + random( Game->MapNumber );
  int HighEnd = random( LowEnd, LowEnd + Game->MapNumber );
  LastRoom = random( LowEnd, HighEnd );

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
  for ( uint8_t a = 0; a < SpriteCap; a++ ) Mobs[a].Game = this;
}
void SquarioGame::NewGame( ) {
  Score = 0;
  DistancePoints = 0;
  Coins = 0;
  Lives = 1;
  MapNumber = 1;
  Player.LoadSprite( SmallSquarioSprite, 10, SpawnY );
  StartLevel( );
}
void SquarioGame::StartLevel( ) {
  Event = ETPlaying;
  SFX = NULL;
  Level.NewMap( );
  while ( Player.Falling() ) Player.Move();
  AdjustCamera( );
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
    if ( Player.Jump() ) SFX = SFX_Jump;
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
    AdjustCamera();
  }
  for ( uint8_t a = 0; a < SpriteCap; a++ ) {
    if ( Mobs[a].Active ) {
      Mobs[a].Think();
      if ( Mobs[a].y > MapPixelHeight ) Mobs[a].Deactivate();
      else if ( TestCollision( &Player, &Mobs[a] ) ) {
        if ( Mobs[a].SpriteData == MushroomSprite ) {
          Mobs[a].Deactivate();
          Score += POINTSMushroom;
          if ( Player.SpriteData == SmallSquarioSprite ) {
            Player.LoadSprite( BigSquarioSprite, Player.x, Player.y-8 );
            SFX = SFX_Mushroom;
          }
        }
        else if ( Player.Falling() ) {
          Mobs[a].Deactivate();
          Score += POINTSKill;
          SFX = SFX_Hit;
          if ( ButtonState[ ButtonJump ] ) { Player.vy = -10; }
          else { Player.vy = -4; }          
        }
        else {
          if ( !EventCounter && Player.Height() == TileSize ) {
            Event = ETDeath;
            EventCounter = 30;
          }
          if ( !EventCounter && Player.Height() > TileSize ) {
            Player.LoadSprite( SmallSquarioSprite, Player.x, Player.y+8 );
            SFX = SFX_Hit;
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
    EventCounter = 0;
    DistancePoints += Player.x / TileSize;
    MapNumber++;
    Player.x = 10;
    Player.y = SpawnY;
    StartLevel( );
  }
}
bool SquarioGame::TestCollision( Sprite * TestSprite1, AISprite * TestSprite2 ) {
  if ( TestRoughCollision( TestSprite1, TestSprite2 ) )
    if ( TestPixelCollision( TestSprite1, TestSprite2 ) ) return true;
  return false;
}
bool SquarioGame::TestRoughCollision( Sprite * TestSprite1, AISprite * TestSprite2 ) {
  if ( TestSprite1->IsIn ( TestSprite2->x,       TestSprite2->y ) ||
       TestSprite1->IsIn ( TestSprite2->RightX(),TestSprite2->y ) ||
       TestSprite1->IsIn ( TestSprite2->x,       TestSprite2->BottomY() ) ||
       TestSprite1->IsIn ( TestSprite2->RightX(),TestSprite2->BottomY() ) ) {
    return true;
  }
  return false;
}
bool SquarioGame::TestPixelCollision( Sprite * TestSprite1, AISprite * TestSprite2 ) {
  for ( int a = max( TestSprite1->x, TestSprite2->x ); a <= min( TestSprite1->RightX(), TestSprite2->RightX() ); a++ ) {
    for ( int b = max( TestSprite1->y, TestSprite2->y ); b <= min( TestSprite1->BottomY(), TestSprite2->BottomY() ); b++ ) {
      if ( TestSprite1->GetPixelAbsolute(a,b) ) {
        if ( TestSprite2->GetPixelAbsolute(a,b) ) return true;
      }
    }
  }
  return false;
}
void SquarioGame::Die( ) {
  Lives--;
  Draw();
  if ( Lives ) {
    Player.x = 10;
    Player.y = SpawnY;
    StartLevel( );
  }
  else {
    DistancePoints += Player.x / TileSize;
    Event = ETOff;
    Display->drawRect(16,8,96,48, WHITE); // Box border
    Display->fillRect(17,9,94,46, BLACK); // Black out the inside
    Display->drawSlowXYBitmap(30,12,gameover,72,14,1);
    
    Display->setCursor(26,29); Display->print(F("Score"));
    Display->setCursor(20,37); Display->print(F("+Dist."));
    Display->setCursor(26,45); Display->print(F("Total"));

    DiePrint( 29, Score );
    DiePrint( 37, DistancePoints );
    Score += DistancePoints;
    DiePrint( 45, Score );
  }
}
void SquarioGame::DiePrint( uint8_t y, unsigned int i ) {
  Display->setCursor( 96 - getTextOffset( i ), y );
  Display->print( i );
  Display->display( );
  delay(250);
  while (!Display->buttonsState());
}
void SquarioGame::DrawMobs( ) {
  for ( uint8_t a = 0; a < SpriteCap; a++ ) {
    if ( Mobs[a].Active ) {
      if ( Mobs[a].RightX() - CameraX > 0 && Mobs[a].x - CameraX < 128 ) Mobs[a].Draw();
    }
  }
}
void SquarioGame::DrawMap( ) {
  int mountainOffset = ( CameraX / 4 ) % 128;
  int mountainYOffset = ( CameraY / 12 ) +20;
  if ( MapNumber % 2 ) {
    //Pyramid-1
  Display->drawLine(  mountainOffset-128 , mountainYOffset+32,  mountainOffset-96 ,mountainYOffset,WHITE);
  Display->drawLine(  mountainOffset-96 , mountainYOffset,  mountainOffset-64 ,mountainYOffset+32,WHITE);

//Palmtree0
  Display->drawLine(  mountainOffset-64 , mountainYOffset+32,  mountainOffset-64 ,mountainYOffset+16,WHITE);//Line up
  Display->drawLine(  mountainOffset-61 , mountainYOffset+16,  mountainOffset-67 ,mountainYOffset+16,WHITE);//top line
  Display->drawLine(  mountainOffset-61 , mountainYOffset+16,  mountainOffset-59 ,mountainYOffset+18,WHITE);//leaf1
  Display->drawLine(  mountainOffset-67 , mountainYOffset+16,  mountainOffset-69 ,mountainYOffset+18,WHITE);//leaf2
  Display->drawLine(  mountainOffset-63 , mountainYOffset+18,  mountainOffset-65 ,mountainYOffset+18,WHITE);//lower line
  Display->drawLine(  mountainOffset-63 , mountainYOffset+18,  mountainOffset-61 ,mountainYOffset+20,WHITE);//leaf1
  Display->drawLine(  mountainOffset-65 , mountainYOffset+18,  mountainOffset-67 ,mountainYOffset+20,WHITE);
  
 //Pyramid0
  Display->drawLine(  mountainOffset-56 , mountainYOffset+32,  mountainOffset-32 ,mountainYOffset+8,WHITE);
  Display->drawLine(  mountainOffset-32 , mountainYOffset+8,  mountainOffset-8 ,mountainYOffset+32,WHITE);
  //Pyramid1
  Display->drawLine(  mountainOffset , mountainYOffset+32,  mountainOffset+32 ,mountainYOffset,WHITE);
  Display->drawLine(  mountainOffset+32 , mountainYOffset,  mountainOffset+64 ,mountainYOffset+32,WHITE);
   
   //Palmtree
  Display->drawLine(  mountainOffset+64 , mountainYOffset+32,  mountainOffset+64 ,mountainYOffset+16,WHITE);
  Display->drawLine(  mountainOffset+61 , mountainYOffset+16,  mountainOffset+67 ,mountainYOffset+16,WHITE);
  Display->drawLine(  mountainOffset+61 , mountainYOffset+16,  mountainOffset+59 ,mountainYOffset+18,WHITE);
  Display->drawLine(  mountainOffset+67 , mountainYOffset+16,  mountainOffset+69 ,mountainYOffset+18,WHITE);
  Display->drawLine(  mountainOffset+63 , mountainYOffset+18,  mountainOffset+65 ,mountainYOffset+18,WHITE);
  Display->drawLine(  mountainOffset+63 , mountainYOffset+18,  mountainOffset+61 ,mountainYOffset+20,WHITE);    
  Display->drawLine(  mountainOffset+65 , mountainYOffset+18,  mountainOffset+67 ,mountainYOffset+20,WHITE);  
    
    //Pyramid2
  Display->drawLine(  mountainOffset+72 , mountainYOffset+32,  mountainOffset+96 ,mountainYOffset+8,WHITE);
  Display->drawLine(  mountainOffset+96 , mountainYOffset+8,  mountainOffset+120 ,mountainYOffset+32,WHITE);
     //Pyramid3
  Display->drawLine(  mountainOffset+128 , mountainYOffset+32,  mountainOffset+156 ,mountainYOffset,WHITE);
  Display->drawLine(  mountainOffset+156 , mountainYOffset,  mountainOffset+194 ,mountainYOffset+32,WHITE);
  }
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
                      if ( !(EventCounter % 2) ) Player.Draw();
                      break;
    case ETPipeDrop:  Player.Draw(); DrawMap(); DrawMobs(); break;
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
void SquarioGame::ActivateButtonCD( ) {
  lastPress = millis();
}
bool SquarioGame::ButtonOffCD( ) {
  if ( millis() > lastPress + BUTTONCD ) return true;
  return false;
}

int SquarioGame::getTextOffset(int s) {
  if (s > 9999) { return 20; }
  if (s > 999) { return 15; }
  if (s > 99) { return 10; }
  if (s > 9) { return 5; }
  return 0;
}


