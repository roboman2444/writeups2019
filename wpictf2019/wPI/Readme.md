# wPI - Reversing - 400 points

I am one of the organizers of WPICTF, so this writeup is going to be a little "biased" per say. Remy wrote the challenge, and asked me to create a solution for it.

If you want a writeup from the point of view of a competitor (no source access, no talking to the creator, etc), then please look at TODO.

## Challenge description:

I wanted to make a binary to show my school spirit, but I think I may have missed the mark...

made by rm -k

File from https://drive.google.com/open?id=1Ysa78UAXp8WvfUPt9NtLyreokoHfINrM

## Lookin at it

Well I had source code access and was also talking to the challenge creator (Remy) during this process.

From the outside, it is a generic reversing challenge. Takes the flag on argv[1] and tells you wheather the flag is correct or incorrect.

There are 3 parts to this challenge.

	The first is a simple md5sum of the entered text, and then uses the first 2 bytes of that md5sum as an initial offset.

	The second part (and meat of the challenge) is to take each set of 2 bytes of the entered flag, add that onto the offset, and then calculate Pi at those offsets. The Pi calculation resulted in 3 hex digits which would be compared to a table. I will call these "cookies".

	The final part is a sha3 of the flag to verify against a known hash. This is to remove any false positives.

## General solution idea


	The initial md5sum offset was designed such to attempt to remove the possibility of bruteforcing the solution 2 bytes at a time.

	Turns out Armia Prezeca had a slightly easier solution to this that could get past the initial md5sum, but this is the "correct" solution.

	Each cookie shows up quite often in Pi, so we cant just look for the first instance of the cookie and use that.

	Simplest Idea: Take the table of Pi "cookies", find everywhere they showed up in Pi, and then DFS combinations of them until the offsets between them equalled the flag (verified by sha3).

	The total search space of pi would be less than ((FLAG_LEN/2)+1) * 32700, as the largest 2 byte "offset" could be 32639 (printable ascii, both byte values have to be < 128). We knew the flag len based on the table of pi "cookies".


## Generating pi
	Fuck that. Pi is hard and slow to generate. I found a pre-made million digis of pi in hex that encompassed my pi search space. Ezpz.

	First i went through that list and kept track of every time the "cookies" showed up. I dont want to search for them in Pi a billion times, so caching the results into a "fast" data structure was key.

	All connections in the search have to go forward (Positive offset), so we can ignore going "backwards" in Pi.

	While doing the DFS, we can ignore connections that would result in unprintable characters (each byte <32 or >127), or that went over the 2 byte bounds (65535); This also further lowered the search space.

	Finally, we had to deal with the "unknown" start offset (md5). We did this by subtracting "WP" from all the instances of the start cookie and using that as our actual starting place in our DFS. We could also early out any 2nd cookies that didnt result in an offset of "I{", but our solver was fast enough without that.

	After some more optimization, we got the solver to run faster than the actual challenge itself.
