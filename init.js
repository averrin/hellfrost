for (var x = 0; x < 100; x++) {
	for (var y = 0; y < 100; y++) {
		var e = registry.create("CRATE", x, y, 0);
		e.select();
	}
	sleep(10);
}







