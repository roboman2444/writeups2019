# wPI - Reversing - 400 points

I am one of the organizers of WPICTF, so this writeup is going to be a little "biased" per say. Remy wrote the challenge, and asked me to create a solution for it.

If you want a writeup from the point of view of a competitor (no source access, no talking to the creator, etc), then please look at [Armia Prezesa's writeup](https://wiki.armiaprezesa.pl/books/wpictf2019/page/wpi)

## Challenge description:

I wanted to make a binary to show my school spirit, but I think I may have missed the mark...

made by rm -k

File from https://drive.google.com/open?id=1Ysa78UAXp8WvfUPt9NtLyreokoHfINrM

## Lookin at it

Well I had source code access and was also talking to the challenge creator (Remy) during this process.

From the outside, it is a generic reversing challenge. Takes the flag on argv[1] and tells you wheather the flag is correct or incorrect.

There are 3 parts to this challenge.

1. The first is a simple md5sum of the entered text, and then uses the first 2 bytes of that md5sum as an initial offset.
2. The second part (and meat of the challenge) is to take each set of 2 bytes of the entered flag, add that onto the offset, and then calculate Pi at those offsets. The Pi calculation resulted in 3 hex digits which would be compared to a table. I will call these "cookies".
3. The final part is a sha3 of the flag to verify against a known hash. This is to remove any false positives.

## General solution idea


The initial md5sum offset was designed such to attempt to remove the possibility of bruteforcing the solution 2 bytes at a time.

Turns out Armia Prezesa had a slightly easier solution to this that could get past the initial md5sum, but this is the "correct" solution. 

Each cookie shows up quite often in Pi, so we cant just look for the first instance of the cookie and use that.

Simplest Idea: Take the table of Pi "cookies", find everywhere they showed up in Pi, and then DFS combinations of them until the offsets between them equalled the flag (verified by sha3).

The total search space of pi would be less than ((FLAG_LEN/2)+1) * 32700, as the largest 2 byte "offset" could be 32639 (printable ascii, both byte values have to be < 128). We knew the flag len based on the table of pi "cookies". 16 "cookies" = 32 total bytes of flag. 5 of which we already know (WPI{...})

This means we need at most 555900 digits of pi... Not too bad.

## Generating pi
Fuck that. Pi is hard and slow to generate. I found a pre-made million digis of pi in hex that encompassed my pi search space. I think i downloaded it from [https://pi2e.ch/blog/2017/03/10/pi-digits-download/](https://pi2e.ch/blog/2017/03/10/pi-digits-download/). Ezpz.

First i went through that list and kept track of every time the "cookies" showed up. I dont want to search for them in Pi a billion times, so caching the results into a "fast" data structure was key. For each cookie, i Kept track a table of locations that it showed up in pi, sorted.

All connections in the search have to go forward (Positive offset), so we can ignore going "backwards" in Pi.

While doing the DFS, we can ignore connections that would result in unprintable characters (each byte <32 or >127), or that went over the 2 byte bounds (65535); This also further lowered the search space.

Finally, we had to deal with the "unknown" start offset (md5). We did this by subtracting "WP" from all the instances of the start cookie and using that as our actual starting place in our DFS. We could also early out any 2nd cookies that didnt result in an offset of "I{", but our solver was fast enough without that.

Finally, we could just use each "end" of the DFS as a string to throw at the final sha3 check. If the check passed, then we would have the flag. There were thousands of strings that passed the Pi part, so the sha3 was very necisary.

After some more optimization, we got the solver to run faster than the actual challenge itself. Can probably speed it up even still by doing some stuff like binary searching, but... its already fast.

```
$ time ./a.out 
e89, 140			// cookie, and number of times it showed up in our Pi we cared about
b87, 145
8ad, 141
7c7, 133
e7c, 133
3a3, 138
425, 138
275, 144
b54, 135
1a6, 131
2ed, 119
1c5, 142
fdf, 137
331, 112
2db, 126
74f, 115
WPI{Sw33TeR_tH@n_aH_cH3rRi3_P1e}


real	0m4.932s
user	0m4.925s
sys	0m0.004s
```

[See solution script](solve.c)
