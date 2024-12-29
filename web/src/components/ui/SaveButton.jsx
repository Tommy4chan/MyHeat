import Button from "./Button";

const SaveButton = ({ onClick, className }) => {
  return (
    <Button buttonText='Зберегти' color='purple' onClick={onClick} className={className} />
  );
};

export default SaveButton;