class Platform {
  private PVector translation, rotation, initialHeight;
  private PVector[] baseJoint, platformJoint, q, l;

  // REAL MEASUREMENTS (inches)
  private final float INITIAL_HEIGHT = 15.46;
  private final float JOINT_HEIGHT = 2.54;
  private final float MATERIAL_THICKNESS = 1.5;
  
  private final float BASE_Z_OFFSET = MATERIAL_THICKNESS / 2 + JOINT_HEIGHT;
  private final float PLAT_Z_OFFSET = -(BASE_Z_OFFSET);
  
  private final int   ACTUATOR_STROKE_LENGTH = 12;
  private final float ACTUATOR_MIN_LENGTH = 17.9;
  private final float ACTUATOR_MAX_LENGTH = 22.9;
  
  // base joint positions relative to base center
  private final float A1_BASE_X = -21.55;
  private final float A1_BASE_Y = -1.5;
  
  private final float A2_BASE_X = 9.47;
  private final float A2_BASE_Y = -19.41;
  
  private final float B1_BASE_X = 12.07;
  private final float B1_BASE_Y = -17.91;
  
  private final float B2_BASE_X = 12.07;
  private final float B2_BASE_Y = 17.91;
  
  private final float C1_BASE_X = 9.47;
  private final float C1_BASE_Y = 19.41;
  
  private final float C2_BASE_X = -21.55;
  private final float C2_BASE_Y = 1.5;
  
  private final float BASE_RADIUS = 21.6;
  
  // platform joint positions relative to platform center
  private final float A1_PLAT_X = -6.93;
  private final float A1_PLAT_Y = -9.00;
  
  private final float A2_PLAT_X = -4.33;
  private final float A2_PLAT_Y = -10.50;
  
  private final float B1_PLAT_X = 11.26;
  private final float B1_PLAT_Y = -1.50;
  
  private final float B2_PLAT_X = 11.26;
  private final float B2_PLAT_Y = 1.50;
  
  private final float C1_PLAT_X = -4.33;
  private final float C1_PLAT_Y = 10.50;
  
  private final float C2_PLAT_X = -6.93;
  private final float C2_PLAT_Y = 9.00;
  
  private final float PLAT_RADIUS = 11.4;
  
  

  public Platform() {
    translation = new PVector();
    rotation = new PVector();
    initialHeight = new PVector(0, 0, INITIAL_HEIGHT);
    q = new PVector[6];
    l = new PVector[6];
    
    baseJoint = new PVector[]{
      new PVector(A1_BASE_X, A1_BASE_Y, BASE_Z_OFFSET),
      new PVector(A2_BASE_X, A2_BASE_Y, BASE_Z_OFFSET),
      new PVector(B1_BASE_X, B1_BASE_Y, BASE_Z_OFFSET),
      new PVector(B2_BASE_X, B2_BASE_Y, BASE_Z_OFFSET),
      new PVector(C1_BASE_X, C1_BASE_Y, BASE_Z_OFFSET),
      new PVector(C2_BASE_X, C2_BASE_Y, BASE_Z_OFFSET),
    };
    
   platformJoint = new PVector[]{
      new PVector(A1_PLAT_X, A1_PLAT_Y, PLAT_Z_OFFSET),
      new PVector(A2_PLAT_X, A2_PLAT_Y, PLAT_Z_OFFSET),
      new PVector(B1_PLAT_X, B1_PLAT_Y, PLAT_Z_OFFSET),
      new PVector(B2_PLAT_X, B2_PLAT_Y, PLAT_Z_OFFSET),
      new PVector(C1_PLAT_X, C1_PLAT_Y, PLAT_Z_OFFSET),
      new PVector(C2_PLAT_X, C2_PLAT_Y, PLAT_Z_OFFSET),
    };
    
    for (int i = 0; i < 6; i++) {
      q[i] = new PVector();
      l[i] = new PVector();
    }

    calcLengths();
  }

  public void applyTranslationAndRotation(PVector t, PVector r) {
    rotation.set(r);
    translation.set(t);
    calcLengths();
  }

  private void calcLengths() {
    for (int i=0; i<6; i++) {
      // rotation
      q[i].x = cos(rotation.z)*cos(rotation.y)*platformJoint[i].x +
        (-sin(rotation.z)*cos(rotation.x)+cos(rotation.z)*sin(rotation.y)*sin(rotation.x))*platformJoint[i].y +
        (sin(rotation.z)*sin(rotation.x)+cos(rotation.z)*sin(rotation.y)*cos(rotation.x))*platformJoint[i].z;

      q[i].y = sin(rotation.z)*cos(rotation.y)*platformJoint[i].x +
        (cos(rotation.z)*cos(rotation.x)+sin(rotation.z)*sin(rotation.y)*sin(rotation.x))*platformJoint[i].y +
        (-cos(rotation.z)*sin(rotation.x)+sin(rotation.z)*sin(rotation.y)*cos(rotation.x))*platformJoint[i].z;

      q[i].z = -sin(rotation.y)*platformJoint[i].x +
        cos(rotation.y)*sin(rotation.x)*platformJoint[i].y +
        cos(rotation.y)*cos(rotation.x)*platformJoint[i].z;

      // translation
      q[i].add(PVector.add(translation, initialHeight));
      l[i] = PVector.sub(q[i], baseJoint[i]);
    }
  }

  public void draw() {
    // draw Base
    noStroke();
    fill(128);
    ellipse(0, 0, 2*BASE_RADIUS, 2*BASE_RADIUS);
    
    for (int i=0; i<6; i++) {
      pushMatrix();
      translate(baseJoint[i].x, baseJoint[i].y, baseJoint[i].z);
      noStroke();
      fill(0);
      ellipse(0, 0, 2, 2);
      textSize(3);
      text(String.format("%.2f", (l[i].mag() - ACTUATOR_MIN_LENGTH) / ACTUATOR_STROKE_LENGTH), 2,0,0);
      popMatrix();
      
      stroke(100);
      strokeWeight(1);
      line(baseJoint[i].x, baseJoint[i].y, baseJoint[i].z, baseJoint[i].x, baseJoint[i].y, 0);
    }

    // draw phone joints and rods
    for (int i=0; i<6; i++) {
      pushMatrix();
      translate(q[i].x, q[i].y, q[i].z);
      noStroke();
      fill(0);
      ellipse(0, 0, 2, 2);
      popMatrix();

      stroke(100);
      strokeWeight(1);
      line(baseJoint[i].x, baseJoint[i].y, baseJoint[i].z, q[i].x, q[i].y, q[i].z);
    }

    // sanity check
    pushMatrix();
    translate(initialHeight.x, initialHeight.y, initialHeight.z);
    translate(translation.x, translation.y, translation.z);
    rotateZ(rotation.z);
    rotateY(rotation.y);
    rotateX(rotation.x);
    stroke(245);
    noFill();
    ellipse(0, 0, 2*PLAT_RADIUS, 2*PLAT_RADIUS);
    popMatrix();
  }
}
