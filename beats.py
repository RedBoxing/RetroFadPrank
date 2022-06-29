import madmom 

proc = madmom.features.beats.DBNBeatTrackingProcessor(fps=100)
act = madmom.features.beats.RNNBeatProcessor()('Retro Fad.mp3')

beat_times = proc(act)

# save beats to file with time code mm:ss:ms knowing that the array is a list of probabilities for each frame, we are only going to take those that are above 80

