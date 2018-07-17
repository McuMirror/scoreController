module player() {
    // Head
    color("brown")
        translate([0, 0, 200])
            sphere(d=30);
    // Collo
    color("pink")
        translate([0, 0, 180])
            cylinder(d=10, h=10);
    // Busto
    color("white")
        translate([0, 0, 110])
            scale([0.5, 1, 1])
                cylinder(d1=50, d2=60, h=70);
    // Pantaloncini
    color("darkgray")
        translate([0, 0, 70])
            scale([0.75, 1, 1])
                cylinder(d=55, h=40);
    // Gambe
    color("pink")
        translate([0,-12.0, 35])
            scale([1, 1, 1])
                cylinder(d2=25, d1=15, h=35);
    color("pink")
        translate([0, 12.0, 35])
            scale([1, 1, 1])
                cylinder(d2=25, d1=15, h=35);
}

/*
union() {
    // Fondo
    color([0.75, 0.75, 1.0])
        cube([9800, 8900, 2.9], center=true);
    
    // Campo di gioco
    color([1.0, 1.0, 0.0])
        cube([1800, 900, 3], center=true);
    
    // Linee
    color("white") {
        cube([40, 900, 3.1], center = true);
        translate([0,-445, 0])
            cube([1800, 40, 3.1], center=true);
        translate([0, 445, 0])
            cube([1800, 40, 3.1], center=true);
        translate([0, 445, 0])
            cube([1800, 40, 3.1], center=true);
           
        translate([295, 0, 0])
            cube([40, 900, 3.1], center = true);
        translate([-295, 0, 0])
            cube([40, 900, 3.1], center = true);
           
        translate([895, 0, 0])
            cube([40, 900, 3.1], center = true);
        translate([-895, 0, 0])
            cube([40, 900, 3.1], center = true);
    }
    // Pali
    color("orange") {
        translate([0,-545, 0])
            cylinder(d=40, h=250);
        translate([0, 545, 0])
            cylinder(d=40, h=250);
    }
    
    // Rete
    color("white") {// Bande
        translate([0, 0, 225])
            cube([1, 1090, 25], center=true);
        translate([0, 0, 125])
            cube([1, 1090, 25], center=true);
         translate([0, 445, 175])
            cube([1, 25, 100], center=true);
         translate([0,-445, 175])
            cube([1, 25, 100], center=true);
  }
  color("red") {
      for(y=[20:40:860]) {
          translate([0, y-440, 175])
            cylinder(d=4, h=100, center=true);
      }
      for(z=[20:40:80]) {
            translate([0, 0, 120+z])
                rotate([90, 0, 0])
                    cylinder(d=4, h=885, center=true);
      }
  }
}
*/
player();