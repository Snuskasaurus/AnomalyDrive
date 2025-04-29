import bpy

# Define a new custom export operator
class CustomFBXExportOperator(bpy.types.Operator):
    bl_idname = "export_scene.custom_fbx"  # Unique identifier used to call this operator
    bl_label = "Custom FBX Export"         # Name displayed in Blender UI
    bl_options = {'REGISTER', 'UNDO'}      # Allow undo/redo in Blender

    # Filepath for the file dialog
    filepath: bpy.props.StringProperty(subtype="FILE_PATH")

    # Main execution logic
    def execute(self, context):
        print("📦 Custom pre-export logic...")

        # Store original object locations so we can restore them later
        selected_objects = context.selected_objects
        old_locations = {
            obj: obj.location.copy()
            for obj in selected_objects if obj.type == 'MESH'
        }

        # Move selected mesh objects to origin (0, 0, 0)
        for obj in selected_objects:
            if obj.type == 'MESH':
                obj.location = (0, 0, 0)

        # Call Blender's original FBX exporter
        bpy.ops.export_scene.fbx(
            filepath=self.filepath,
            use_selection=True  # Only export selected objects
        )

        # Restore original object locations
        for obj, loc in old_locations.items():
            obj.location = loc

        # Display message in Blender UI
        self.report({'INFO'}, "✅ Custom export finished.")
        return {'FINISHED'}

    # This runs when the user activates the operator (opens file dialog)
    def invoke(self, context, event):
        context.window_manager.fileselect_add(self)  # Opens file browser
        return {'RUNNING_MODAL'}

# Function to add our custom exporter into the File → Export menu
def export_menu_replace(self, context):
    self.layout.operator(CustomFBXExportOperator.bl_idname, text="FBX (with Pre/Post logic)")

# Register all classes and menu hooks
def register():
    bpy.utils.register_class(CustomFBXExportOperator)
    bpy.types.TOPBAR_MT_file_export.append(export_menu_replace)  # Add our exporter to File → Export menu

# Unregister everything when disabled
def unregister():
    bpy.utils.unregister_class(CustomFBXExportOperator)
    bpy.types.TOPBAR_MT_file_export.remove(export_menu_replace)

# Auto-register if running the script directly
if __name__ == "__main__":
    register()