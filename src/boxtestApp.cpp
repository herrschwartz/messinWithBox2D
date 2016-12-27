#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Rand.h"
#include <cstdint>

#include <Box2D/Box2D.h>

using namespace ci;
using namespace ci::app;
using namespace std;


const float BOX_SIZE = 10;

class boxtestApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
    void keyDown( KeyEvent event ) override;
	void update() override;
	void draw() override;
	void addBigBox( const vec2 &pos );
	void addBox( const vec2 &pos, const vec2 vel );
    void addGravityWell( const vec2 &pos );
	
	b2World				*mWorld;
	vector<b2Body*>		mBoxes;
    vector<b2Body*>     walls;
    vector<b2Body*>     circles;
    Rand *gen = new Rand();
    b2Body*             player;

};

void boxtestApp::setup()
{
    gen->randomize();
	b2Vec2 gravity( 0.0f, 0.0f );
	mWorld = new b2World( gravity );
    
    addGravityWell(vec2(0,0));
    addBox(vec2(getWindowCenter().x+160,getWindowCenter().y), vec2(0,5));
    addBox(vec2(getWindowCenter().x+80,getWindowCenter().y), vec2(0,3));
    addBox(vec2(getWindowCenter().x+200,getWindowCenter().y), vec2(0,12));


    
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set( 0.0f, getWindowHeight() );
	b2Body* groundBody = mWorld->CreateBody( &groundBodyDef );
    
    /*
    b2BodyDef ceilingBodyDef;
    groundBodyDef.position.Set( 0.0f, 0.0f );
    b2Body* ceilingBody = mWorld->CreateBody( &groundBodyDef );
    
    b2BodyDef leftWallBodyDef;
    groundBodyDef.position.Set( 0.0f, 0.0f);
    b2Body* leftWallBody = mWorld->CreateBody( &groundBodyDef );
    
    b2BodyDef rightWallBodyDef;
    groundBodyDef.position.Set( getWindowWidth(), 0.0f);
    b2Body* rightWallBody = mWorld->CreateBody( &groundBodyDef );

	// Define the ground box shape.
	
    b2PolygonShape ceilingBox;
    b2PolygonShape leftWallBox;
    b2PolygonShape rightWallBox;

	// The extents are the half-widths of the box.
	
    ceilingBox.SetAsBox(getWindowWidth(), 10.0f);
    leftWallBox.SetAsBox(10.0f, getWindowHeight());
    rightWallBox.SetAsBox(10.0f, getWindowHeight());
	// Add the ground fixture to the ground body.

    ceilingBody->CreateFixture(&ceilingBox, 0.0f);
    leftWallBody->CreateFixture(&leftWallBox, 0.0f);
    rightWallBody->CreateFixture(&rightWallBox, 0.0f);
    
    //b2Body *Player = mWorld->CreateBody(<#const b2BodyDef *def#>)
   
    walls.push_back(ceilingBody);
     */
    b2PolygonShape groundBox;
    groundBox.SetAsBox( getWindowWidth(), 10.0f );
    groundBody->CreateFixture( &groundBox, 0.0f );
    walls.push_back(groundBody);
    
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set( 600, getWindowHeight()-20 );
    
    b2Body *body = mWorld->CreateBody( &bodyDef );
    
    b2PolygonShape dynamicBox;
    
    
    int boxScale=5;

    
    console()<<boxScale<<std::endl;
    dynamicBox.SetAsBox( BOX_SIZE + boxScale , BOX_SIZE + boxScale );
    
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1;
    fixtureDef.friction = 0.3f;
    fixtureDef.restitution = 0; // bounce
    
    long boxSize = BOX_SIZE+boxScale;
    body->SetUserData((void*)boxSize);
    
    body->CreateFixture( &fixtureDef );
    player = body;
}

void boxtestApp::addBox( const vec2 &pos, const vec2 vel )
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set( pos.x, pos.y );
    bodyDef.linearVelocity.Set(vel.x, vel.y);

	b2Body *body = mWorld->CreateBody( &bodyDef );

	b2PolygonShape dynamicBox;
    
   
    int boxScale= gen->nextInt()%7;
    float bnd = gen->nextFloat(.5, 1.4);

    console()<<boxScale<<std::endl;
	dynamicBox.SetAsBox( BOX_SIZE + boxScale , BOX_SIZE + boxScale );

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = bnd;
	fixtureDef.friction = 0.3f;
	fixtureDef.restitution = bnd; // bounce
    
    long boxSize = BOX_SIZE+boxScale;
    body->SetUserData((void*)boxSize);

	body->CreateFixture( &fixtureDef );
	mBoxes.push_back( body );
}
void boxtestApp::addBigBox( const vec2 &pos )
{
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set( pos.x, pos.y );
    bodyDef.linearVelocity.Set(0, 0);
    
    b2Body *body = mWorld->CreateBody( &bodyDef );
    
    b2PolygonShape dynamicBox;
    
    int boxScale = 20;
    
    console()<<boxScale<<std::endl;
    dynamicBox.SetAsBox( BOX_SIZE + boxScale , BOX_SIZE + boxScale );
    
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 3.0;
    fixtureDef.friction = 0.3f;
    fixtureDef.restitution = .9; // bounce
    
    long boxSize = BOX_SIZE+boxScale;
    body->SetUserData((void*)boxSize);
    
    body->CreateFixture( &fixtureDef );
    mBoxes.push_back( body );
}
void boxtestApp::addGravityWell( const vec2 &pos){
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set( getWindowCenter().x, getWindowCenter().y );
    bodyDef.linearVelocity.Set(0, 0);
    
    b2Body *body = mWorld->CreateBody(&bodyDef);
    
    b2CircleShape circle;
    circle.m_radius = 28.0f;
    circle.m_p.Set(0, 0);
    b2FixtureDef myFixtureDef;
    myFixtureDef.shape = &circle;
    myFixtureDef.density = 100;
    myFixtureDef.friction = 0.3f;
    myFixtureDef.restitution = .9; // bounce
    body->CreateFixture(&myFixtureDef);
    circles.push_back(body);
    
}

void boxtestApp::mouseDown( MouseEvent event )
{
    if(event.isLeft()&& !event.isShiftDown()){
	addBox( event.getPos(), vec2(gen->nextInt()%3, gen->nextInt()%3) );
    }
    else if(event.isRight() && !event.isShiftDown()){
        addBigBox(event.getPos());
    } else if(event.isLeft() && event.isShiftDown()){
        console()<<"adding well"<<std::endl;
        addGravityWell(event.getPos());
    }
}

void boxtestApp::keyDown( KeyEvent event ) {
    if( event.getChar() == 'a' ){
        console()<<"You pressed the 'a' key \n";
    } else if( event.getChar() == 'd' ){
        
    }
}

void boxtestApp::update()
{
    for(const auto &circle : circles){
        for(const auto &box : mBoxes){
            b2Vec2 circlePos = circle->GetWorldCenter();
            b2Vec2 boxPos = box->GetWorldCenter();
            b2Vec2 distance = b2Vec2(0,0);
            distance.operator-=(boxPos);
            distance.operator+=(circlePos);
            distance.operator*=(2);
            box->ApplyForce(distance, box->GetWorldCenter());
        }
    }
    player->ApplyForce(b2Vec2(0,-9), player->GetWorldCenter());
	for( int i = 0; i < 10; ++i )
		mWorld->Step( 1 / 30.0f, 10, 10 );

}

void boxtestApp::draw()
{
    	gl::clear();
    gl::color(.7, .1, .5);
    for( const auto &circle : circles){
        gl::pushModelMatrix();
        gl::drawSolidCircle(vec2(circle->GetPosition().x, circle->GetPosition().y), 28);
        gl::popModelMatrix();
    }
    gl::color(.1, 0, .9);
    gl::pushModelMatrix();
    gl::translate(player->GetPosition().x, player->GetPosition().y);
    gl::rotate(player->GetAngle());
    long boxSize = (long)player->GetUserData();
    gl::drawSolidRect( Rectf( -boxSize, -boxSize, boxSize, boxSize ) );
    gl::popModelMatrix();
	
	gl::color( 1, 0.5f, 0.25f );
	for( const auto &box : mBoxes ) {
		gl::pushModelMatrix();
		gl::translate( box->GetPosition().x, box->GetPosition().y );
		gl::rotate( box->GetAngle() );
        long boxSize = (long)box->GetUserData();
		gl::drawSolidRect( Rectf( -boxSize, -boxSize, boxSize, boxSize ) );
		gl::popModelMatrix();
	}
    
        gl::color(1, 0, .5);
        auto wall = walls[0];
        gl::pushModelMatrix();
        gl::translate(wall->GetPosition().x, wall->GetPosition().y);
        gl::rotate(wall->GetAngle());
        gl::drawSolidRect(Rectf(-10,-10, getWindowWidth(), getWindowWidth()));
        gl::popModelMatrix();
    /*
    auto ceiling = walls[1];
        gl::pushModelMatrix();
        gl::translate(ceiling->GetPosition().x, ceiling->GetPosition().y);
        gl::rotate(ceiling->GetAngle());
        gl::drawSolidRect(Rectf(-10,-10, getWindowWidth(), 10));
        gl::popModelMatrix();
     */
}



CINDER_APP( boxtestApp, RendererGl(), [&]( App::Settings *settings ) {
    settings->setWindowSize( 1280, 720 );
})
