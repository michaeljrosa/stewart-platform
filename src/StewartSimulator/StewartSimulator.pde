import peasy.*; //<>//
import controlP5.*;

float MAX_TRANSLATION = 10;
float MIN_Z = -1.7;
float MAX_Z = 16.07;
float MAX_ROTATION = PI/6;

ControlP5 cp5;
PeasyCam camera;
Platform mPlatform;

float posX=0, posY=0, posZ=0, rotX=0, rotY=0, rotZ=0;
boolean ctlPressed = false;

void setup() {
  size(1024, 768, P3D);
  smooth();
  frameRate(60);
  textSize(10);

  camera = new PeasyCam(this, 150);
  camera.setRotations(-1.0, 0.0, 0.0);
  camera.lookAt(0.0, -40.0, -20.0);

  mPlatform = new Platform();
  mPlatform.applyTranslationAndRotation(new PVector(), new PVector());

  cp5 = new ControlP5(this);

  cp5.addSlider("posX")
    .setPosition(20, 20)
    .setSize(180, 40).setRange(-MAX_TRANSLATION, MAX_TRANSLATION);
  cp5.addSlider("posY")
    .setPosition(20, 70)
    .setSize(180, 40).setRange(-MAX_TRANSLATION, MAX_TRANSLATION);
  cp5.addSlider("posZ")
    .setPosition(20, 120)
    .setSize(180, 40).setRange(MIN_Z, MAX_Z);

  cp5.addSlider("rotX")
    .setPosition(width-200, 20)
    .setSize(180, 40).setRange(-1, 1);
  cp5.addSlider("rotY")
    .setPosition(width-200, 70)
    .setSize(180, 40).setRange(-1, 1);
  cp5.addSlider("rotZ")
    .setPosition(width-200, 120)
    .setSize(180, 40).setRange(-1, 1);

  cp5.setAutoDraw(false);
  camera.setActive(true);
}

void draw() {
  background(200);
  mPlatform.applyTranslationAndRotation(new PVector(posX, posY, posZ), 
    PVector.mult(new PVector(rotX, rotY, rotZ), MAX_ROTATION));
  mPlatform.draw();

  hint(DISABLE_DEPTH_TEST);
  camera.beginHUD();
  cp5.draw();
  camera.endHUD();
  hint(ENABLE_DEPTH_TEST);
}

void controlEvent(ControlEvent theEvent) {
  camera.setActive(false);
}

void mouseReleased() {
  camera.setActive(true);
}

void mouseDragged () {
  if (ctlPressed) {
    posX = map(mouseX, 0, width, -1, 1);
    posY = map(mouseY, 0, height, -1, 1);
  }
}

void keyPressed() {
  if (key == ' ') {
    camera.setRotations(-1.0, 0.0, 0.0);
    camera.lookAt(0.0, -40.0, -20.0);
    camera.setDistance(150);
  } else if (keyCode == CONTROL) {
    camera.setActive(false);
    ctlPressed = true;
  }
}

void keyReleased() {
  if (keyCode == CONTROL) {
    camera.setActive(true);
    ctlPressed = false;
  }
}
