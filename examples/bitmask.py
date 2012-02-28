int index = 0;

if (p.x > splitX) {

    index |= 1;

}

if (p.y > splitY) {

    index |= 2;

}

if (p.z > splitZ) {

    index |= 4;

}