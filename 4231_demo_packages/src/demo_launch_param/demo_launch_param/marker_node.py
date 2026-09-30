import rclpy
from rclpy.node import Node
from visualization_msgs.msg import Marker
from geometry_msgs.msg import Point
from std_msgs.msg import ColorRGBA

class MarkerNode(Node):
    def __init__(self, namespace):
        super().__init__('marker_node', namespace=namespace)
        self.marker_pub = self.create_publisher(Marker, 'visualization_marker', 10)
        self.timer = self.create_timer(1.0, self.publish_marker)

        # Retrieve parameters from the parameter server
        self.declare_parameter('x', 0.0)
        self.declare_parameter('color', [1.0, 0.0, 0.0, 1.0])
        self.x = self.get_parameter('x').value
        self.color = self.get_parameter('color').value

    def publish_marker(self):
        self.x = self.get_parameter('x').value
        self.color = self.get_parameter('color').value
        marker_msg = Marker()
        marker_msg.header.frame_id = 'dynamic_frame'
        marker_msg.header.stamp = self.get_clock().now().to_msg()
        marker_msg.type = Marker.SPHERE
        marker_msg.action = Marker.ADD
        marker_msg.pose.position.x = self.x
        marker_msg.pose.position.y = 0.0
        marker_msg.pose.position.z = 0.0
        marker_msg.scale.x = 0.1
        marker_msg.scale.y = 0.1
        marker_msg.scale.z = 0.1

        # Create a ColorRGBA message and set its values
        marker_msg.color = ColorRGBA()
        marker_msg.color.r = self.color[0]
        marker_msg.color.g = self.color[1]
        marker_msg.color.b = self.color[2]
        marker_msg.color.a = self.color[3]

        self.marker_pub.publish(marker_msg)

def main(args=None):
    rclpy.init(args=args)

    # Get parameters or use defaults
    namespace = 'default'  # Default namespace

    if args is not None:
        namespace = args[1]

    node = MarkerNode(namespace)

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass

    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
