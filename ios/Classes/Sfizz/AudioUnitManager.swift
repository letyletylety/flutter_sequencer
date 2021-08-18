/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Non-UI controller object used to manage the interaction with the AUv3FilterDemo audio unit.
*/

import Foundation
import AudioToolbox
import CoreAudioKit
import AVFoundation


// Controller object used to manage the interaction with the audio unit and its user interface.
public class AudioUnitManager {

    /// The user-selected audio unit.
    private var audioUnit: SfizzAU?

    // The AudioComponentDescription matching the AUv3FilterExtension Info.plist
    private var componentDescription: AudioComponentDescription = {

        // Ensure that AudioUnit type, subtype, and manufacturer match the extension's Info.plist values
        var componentDescription = AudioComponentDescription()
        componentDescription.componentType = kAudioUnitType_MusicDevice
        componentDescription.componentSubType = 0x666c7472 /*'fltr'*/
        componentDescription.componentManufacturer = 0x44656d6f /*'Demo'*/
        componentDescription.componentFlags = 0
        componentDescription.componentFlagsMask = 0

        return componentDescription
    }()

    private let componentName = "Demo: AUv3FilterDemo"

    public init() {

        /*
         Register our `AUAudioUnit` subclass, `AUv3FilterDemo`, to make it able
         to be instantiated via its component description.

         Note that this registration is local to this process.
         */
        AUAudioUnit.registerSubclass(SfizzAU.self,
                                     as: componentDescription,
                                     name: componentName,
                                     version: UInt32.max)

        /*
        AVAudioUnit.instantiate(with: componentDescription) { audioUnit, error in
            guard error == nil, let audioUnit = audioUnit else {
                fatalError("Could not instantiate audio unit: \(String(describing: error))")
            }
            self.audioUnit = audioUnit.auAudioUnit as? SfizzAU
            self.connectParametersToControls()
            self.playEngine.connect(avAudioUnit: audioUnit)
            
            self.loadFile()
        }
        */
    }
    
    private func loadFile() {
        let name = "000_Piano 1"
        let ext = "sfz"
        
        guard let path = Bundle(for: type(of: self)).path(forResource: name, ofType: ext) else {
            fatalError("SFZ file not found.")
        }

        guard self.audioUnit?.loadFile(path: path) == true else {
            fatalError("Sfizz failed to load file")
        }
    }
}
