
CREATE TABLE IF NOT EXISTS channels(
    id INTEGER PRIMARY KEY,
    channel_id TEXT NOT NULL,
    url TEXT NOT NULL,
    title TEXT,
    description TEXT,
    lastOnline DATETIME
);

CREATE TABLE IF NOT EXISTS channel_images(
    id INTEGER NOT NULL REFERENCES channels(id),
    img BLOB,
    preview BLOB
);


--Insert default image
INSERT INTO channels(channel_id, url) VALUES ('__default__', '');

--Load image from program
--INSERT INTO channel_images(id, img) VALUES((SELECT id FROM channels WHERE channel_id = '__default__'), ... );




