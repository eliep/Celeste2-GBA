"""
This is a little script to help determine the possible tracker settings to 
convert a wav file to a impulse tracker (.it). 
I don't actually know anything about trackers, except that they are like
MIDIs, but with included instruments (samples). I have no idea what the settings
in OpenMPT actually do, but I was able to find some formulas for the settings:
https://wiki.openmpt.org/Manual:_Song_Properties#Tempo_Mode
row duration = (2.5 seconds / Tempo) x Speed # I guess Speed = ticks per row

This script bruteforces possible settings for the tracker.

Prints how many rows a sample will play for, what the tempo and 
ticks-per-row settings should be set to and how long the 
sample would actually play for (Or something like that). 
You should use the values of the row with the length, 
that is nearest to the actual song length.

Also make sure the song is not too long. The GBA won't play it then.
OpenMPT will show, that the sample has a high length value
In that case you must split the song up with Audacity or so.
If the song loops, you might need to split it into equally sized parts.

For example MUS_38.it, which only uses snd53.wav, which is 34,525s long.

I split it into 5 equal parts of length 6,525s using Audacity. 
I set x to 6.905 and least_possible_diff to something slightly less than x.
The script prints the following possible combinations of settings:
rows:4, tempo:42, tpr:29, 6.904761904761904
rows:6, tempo:63, tpr:29, 6.904761904761904
rows:7, tempo:71, tpr:28, 6.901408450704226
rows:8, tempo:84, tpr:29, 6.904761904761904
rows:9, tempo:88, tpr:27, 6.903409090909091

Now I use the setting with the length, that is closest to my actual song length.
I only search for lengths lower than my actual song length, because in my testing
the audio had hiccups whenever there was any gap between the samples.
If there are multiple combinations with the same highest value, I choose the one 
with the lowest row count. In this case that would be the first row.

If your song is composed of samples of different sizes, you could try and use
the values for the sample with the highest length. Worked for MUS_40.it, but I
have no idea if that is a universal or clean solution.
"""

# Actual length of the song. Check with MediaViewer or Audacity (better check at ms precision).
x = 3.718
# I made this adjustable, so that you can easily filter out a lot of values
least_possible_diff = 3.71 # Should be a little less than x.
max_rows = 1 # rows per line of samples
max_tempo = 255# "Initial tempo" # XM max: 1000, IT max: 255
max_tpr = 255 # "Ticks/Row" # XM max: 31, IT max:255
for rows in range(1,max_rows+1):
	for tempo in range(32, max_tempo+1):
		for tpr in range(1,max_tpr+1): # 32 seems to be max ## update: i can go higher???
			if (rows*2.5/tempo*tpr) <=x and (rows*2.5/tempo*tpr) > least_possible_diff:
				print(f"rows:{rows}, tempo:{tempo}, tpr:{tpr}, {(rows*2.5/tempo*tpr)}")
# still need to adjust sample :)=)))))

input()